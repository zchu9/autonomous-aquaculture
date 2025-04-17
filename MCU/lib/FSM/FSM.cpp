#include "FSM.h"

#define DEBUG_LIFT 1

/**
 * @brief
 *
 */
void FSM(data &d)
{
    // this should be in the comms handler.
    d.lora->receiveMsg(d.doc);
    checkPowerHandler(d);
    commsHandler(d);

#if DEBUG
    debug_sim ds;
    std::vector<std::string> params;

    // battery
    ds.battery = false;
    char buf[30];
    double num = analogRead(A3);
    num /= analogRead(A4);
    num *= 14.0;
    sprintf(buf, "b=%f", num);
    params.push_back(buf);

    ds.height = true;
    ds.temp = true;
    ds.solar = true;

    updateTime(d);
    int interval = 3;

    if (d.t.seconds != d.last && d.t.seconds % interval == 0) {
        testState(d, ds, params);
        d.last = d.t.seconds;
    }

    params.clear();
#endif
}

#pragma region Initialize

/**
 * @brief initializes the system at startup. The system should be put into low power mode and no connection mode.\n
 * The data struct should be initialized\n
 * The power check system should be initialized\n
 * The comms establish connection Protocol should be initialized\n
 * The emergency lift lowering protocol should be initialized\n
 *
 * @param d the data struct that will be passed from the main function.
 */
void initializeStartup(data &d)
{
#if DEBUG
    initializeDebug();
    initializeNormalFSM(d);
    Serial.println("Hard Coded into normal mode");
#endif

    // uartSwitch(RADIO, 9600, SERIAL_8N1);
    // should probably be a init data function.

    // init the lora class
    d.lora = new LoraRadio;
    d.winch = new winchData(LIFT_PIN, LOWER_PIN, A0);
    d.liftFlag[0] = 0;

    d.cam = new CameraHandler;
    d.cam->begin();

    // initializes the check power interrupt the comms handler, and the emergency lift lowering timer interrupt
    // initializeLPMandNCM(d);
}

/**
 * @brief Initialize the conditions for the Normal FSM\n
 * The mode variables should be set to 0\n
 * The check power interrupt should already be initialized\n
 *
 * if NCM was 1, then a "connected" initialization might be needed\n
 * TODO: Come up with a better name for the "connected" initialization
 */
void initializeNormalFSM(data &d)
{
    // NCM -> Normal
    d.state = NORMAL;
}

/**
 * @brief initialize serial communication
 */
void initializeDebug() {
    Serial.begin(9600);
    Serial1.begin(9600, SERIAL_8N1);
    timerInit();    // should be a normal startup component;
    Serial.println("Debugging Initialized");
}

#pragma endregion Initialize

void sleep()
{
    // Sleep until an interrupt occurs
    // asm - tells compiler this is inline assembly
    // __volatile__ - tells compiler this code has side effects that should not be optimized away
    // "wfi" - wait for interrupt ~ enters low power / sleep mode
    //__asm__ __volatile__("wfi");
}

void checkPowerHandler(data &d)
{
    if (getPowerFlag() == 1)
    {
#if DEBUG
        Serial.println("Power check function");
#endif
        setPowerFlag(false);
        double battVoltage = checkPower(d);
        if(battVoltage < POWER_THRESHOLD){
            getIntoLowPowerMode(d);
            return;
        }

        if(battVoltage >= POWER_THRESHOLD && d.state == LOW_POWER) {
            getOutOfLowPowerMode(d);
        }
    }
}

//////////////////////////////////////////
//          Comms stuff
//////////////////////////////////////////

#pragma region Comms

void commsHandler(data &d)
{
    if (getCommsFlag() == 1)
    {
#if DEBUG
        Serial.println("Comms Module Interrupt");
#endif
        setCommsFlag(false);
        d.lora->receiveMsg(d.doc);
        runCommands(d);
    }
}

int runCommands(data &d)
{
    if (d.doc.isNull())
    {
        Serial.println("No valid JSON received");
        return -1;
    }
    if (d.doc["command"] == 1)
    {
        d.winch->lift(3);
        // winchControl(d);
        Serial.println(">>>>>Lift command received");
    }
    if (d.doc["command"] == 0)
    {
        d.winch->lower(0.8);
        // winchControl(d);
        Serial.println(">>>>>Lower command received");
    }
    // clear the json doc
    d.doc.clear();
    return 0;
}

JsonDocument jsonify(data &d)
{
    // Clear the previous document

    JsonDocument doc;

    // Set the state
    doc["state"] = d.state;

    // Set the power vector
    JsonArray powerArray = d.doc["power"].to<JsonArray>();
    for (size_t i = 0; i < d.power.size(); i++)
    {
        powerArray.add(d.power[i]);
    }

    // Set the temperature readings
    JsonArray tempArray = d.doc["temp"].to<JsonArray>();
    for (size_t i = 0; i < d.temp.size(); i++)
    {
        tempArray.add(d.temp[i]);
    }

    return doc;
}

bool sendData(data &d)
{
    // Convert the data struct to JSON
    JsonDocument doc = jsonify(d);
    size_t len = measureJson(doc);
    char *buffer = new char[len + 1]; // +1 for null terminator
    serializeJson(doc, buffer, len + 1);
    // Send the JSON over LoRa
    bool success = d.lora->sendPackets(buffer);
    Serial.print("made it out 100");
    delete[] buffer; // Free the allocated memory
    return success;
}

#pragma endregion Comms

void getIntoLowPowerMode(data &d)
{
    if (d.state == NORMAL)
    {
        d.state = LOW_POWER;
    }
    else
    {
        d.state = LOW_POWER_NO_CONNECTION;
    }
}

void getOutOfLowPowerMode(data &d)
{
    if (d.state == LOW_POWER_NO_CONNECTION)
    {
        d.state = NO_CONNECTION;
    }
    else
    {
        d.state = NORMAL;
    }
}

void powerStateChange(data &d)
{
    bool already_in_lpm = d.state == LOW_POWER || d.state == LOW_POWER_NO_CONNECTION;
    int temporary = 0;
    if (temporary < POWER_THRESHOLD)
    {
        if (!already_in_lpm)
        {
            getIntoLowPowerMode(d);
        }
    }
    else
    {
        if (already_in_lpm)
        {
            getOutOfLowPowerMode(d);
        }
    }
}

#pragma region Sensors

bool getAndSendImg(data &d)
{
    uint32_t imgSize = d.cam->captureImage();
    // uint32_t imgSize = d.cam->myCAM->read_fifo_length();

    Serial.print("Image size: ");
    Serial.println(imgSize);

    uint32_t imDivider = 6;
    int chunkSize = imgSize / imDivider;
    uint8_t img[chunkSize];

    int numBytesRead = 1;
    int currentChunk = 0;

    if (imgSize > 0)
    {
        d.cam->startImageStream(); // this may cause the beginning to repeat.

        while (numBytesRead > 0)
        {
            currentChunk++;

            numBytesRead = d.cam->readImageChunk(chunkSize, img);
            
            int encodedLength = Base64.encodedLength(chunkSize);
            char encodedImg[encodedLength + 1];

            Base64.encode(encodedImg, (char *)img, chunkSize);

            if (d.lora->sendPackets(encodedImg))
            {
                Serial.println("Chunk:" + String(currentChunk + 1) + " sent");
            }
            else
            {
                Serial.println("Failed to send chunk:" + String(currentChunk + 1));
                d.cam->finishImageStream(); // returns CS line high on failure.
                return false;
            }
        }
    }
    else
    {
        Serial.println("Failed to capture image.");
        d.cam->finishImageStream();
        return false;
    }

    d.cam->finishImageStream();
    return true;
}

void updateTemp(data &d)
{
    // Read temperature
    // float currentTempC = getTempC();
    float currentTempF = getTempF();

    // Print the reading
    Serial.print("Temperature: ");
    // Serial.print(currentTempC);
    // Serial.print(" °C / ");
    Serial.print(currentTempF);
    Serial.println(" °F");

    // Append the reading to the temperature array in d.
    // d.temp.push_back(currentTempC);
    d.temp.push_back(currentTempF);
}

#pragma endregion Sensors

////////////////////////////////////////////////////////////////////////////
// Zach's House
////////////////////////////////////////////////////////////////////////////

#pragma region Debug

void updateTime(data& d) {
    time t = getTime();
    d.t.minutes = t.minutes;
    d.t.seconds = t.seconds;
}

void testState(data& d, debug_sim ds, std::vector<std::string> params) {
    char buffer[80 * 24];

    parseParams(d, ds, params);

    char output[] = "\033[38;5;%d;80;80mCurrent state : %d\n"
    "Height: %0.2f\t|\tTemp: %0.2f\n"
    "Solar_V: %0.2f\t|\tBatt_V: %0.2f\n"
    "Uptime: %2d:%2d\n"
    "Last Transmission time:%2d:%2d\n\033[0m";

    static int color = 16;
    color += 31;    // comment this out if you hate fun :(

    sprintf(buffer, output, \
        color, d.state, \
        d.temp, d.height[0], \
        d.powerData.solarPanelVoltage, d.powerData.batteryVoltage, \
        d.t.minutes, d.t.seconds, \
        0, 0
    );
    Serial.println(buffer);
}

void parseParams(data& d, debug_sim ds, std::vector<std::string> params) {
    for (std::string s : params) {
        std::string w = s.substr(s.find('=')+1);
        char* ptr;
        double value = strtod(w.c_str(),&ptr);
        switch (s[0]) {
        case 'h':
            d.height[0] = value;
            ds.height = false;
            break;
        case 't':
            d.temp[0] = value;
            ds.temp = false;
            break;
        case 'b':      
            d.powerData.batteryVoltage = value;
            ds.battery = false;
            break;
        case 's':
            d.powerData.solarPanelVoltage = value;
            ds.solar = false;
            break;
        }
    }

    // default values to set;
    if (ds.battery) { d.powerData.batteryVoltage = 12.6; }
    if (ds.height) { d.height[0] = 3.0; }
    if (ds.temp) { d.temp[0] = 76.0; }
    if (ds.solar) { d.powerData.solarPanelVoltage = 12.7; }
};

#pragma endregion Debug

double checkPower(data &d)
{
    // getPowerInfo();
    return d.powerData.batteryVoltage;
};
/*
      `'::::.
        _____A_
       /      /\
    __/__/\__/  \___
---/__|" '' "| /___/\----
   |''|"'||'"| |' '||
   `""`""))""`"`""""`
*/