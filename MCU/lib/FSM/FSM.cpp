#include "FSM.h"

#define DEBUG_LIFT 1

static int commsFlag = 0;
static bool lowPowerMode = false;
static bool noConnectionMode = false;
/**
 * @brief
 *
 */
void FSM(data &d)
{
    // this should be in the comms handler.
    checkPowerHandler(d);
    loraListen(d);
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
#endif

    attachInterrupt(digitalPinToInterrupt(RX_INTERRUPT), commsHandler, RISING);

    // init the lora class
    d.lora = new LoraRadio;
    noConnectionMode = d.lora->sendHandshake();

    d.winch = new winchData(LIFT_PIN, LOWER_PIN, A0);

    d.cam = new CameraHandler;
    d.cam->begin();

    // initializes the check power interrupt the comms handler, and the emergency lift lowering timer interrupt
    // initializeLPMandNCM(d);
}

/**
 * @brief initialize serial communication
 */
void initializeDebug()
{
    Serial.begin(9600);
    Serial1.begin(9600, SERIAL_8N1);
    timerInit(); // should be a normal startup component;
    Serial.println("Debugging Initialized");
}

#pragma endregion Initialize

#pragma region Power

void checkPowerHandler(data &d)
{
    if (getPowerFlag() == 1)
    {
#if DEBUG
        Serial.println("Power check function");
#endif
        setPowerFlag(false);
        double battVoltage = checkPower(d);
        if (battVoltage < POWER_THRESHOLD)
        {
            lowPowerMode = true;
            return;
        }

        if (battVoltage >= POWER_THRESHOLD)
        {
            lowPowerMode = false;
        }
    }
}

#pragma endregion Power

//////////////////////////////////////////
//          Comms stuff
//////////////////////////////////////////

#pragma region Comms
void commsHandler()
{
    if (commsFlag == 0)
    {
        commsFlag = 1;
    }
}

void loraListen(data &d)
{
    if (commsFlag == 1)
    {
#if DEBUG
        Serial.println("Comms Module Interrupt");
#endif
        detachInterrupt(digitalPinToInterrupt(RX_INTERRUPT));
        d.lora->receiveMsg(d.doc);
        runCommands(d);
        commsFlag = 0;
        attachInterrupt(digitalPinToInterrupt(RX_INTERRUPT), commsHandler, RISING);
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
        if (lowPowerMode == true)
        {
            sendError(d);
        }
        else
        {
            d.winch->lift(1);
        }
        Serial.println(">>>>>Lift command received");
    }
    if (d.doc["command"] == 0)
    {
        if (lowPowerMode == true)
        {
            sendError(d);
        }
        else
        {
            d.winch->lift(0.8);
        }
        Serial.println(">>>>>Lower command received");
    }
    // clear the json doc
    d.doc.clear();
    return 0;
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
    if (success)
    {
        // clear the data struct
    }
    delete[] buffer; // Free the allocated memory
    return success;
}

bool sendError(data &d)
{
    // Convert the data struct to JSON
    JsonDocument doc = jsonify(d);
    doc["state"] = "Winch Operation Error: Low Power Mode";
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

#pragma region Helpers
JsonDocument jsonify(data &d)
{
    // Clear the previous document

    JsonDocument doc;

    // Set the state
    /*
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
*/
    return doc;
}

std::string getState()
{
    if (lowPowerMode && noConnectionMode)
    {
        return "Low Power No Connection";
    }
    else if (lowPowerMode)
    {
        return "Low Power";
    }
    else if (noConnectionMode)
    {
        return "No Connection";
    }
    else
    {
        return "Normal";
    }
}

#pragma endregion Helpers

////////////////////////////////////////////////////////////////////////////
// Zach's House
////////////////////////////////////////////////////////////////////////////

#pragma region Debug

void updateTime(data &d)
{
    time t = getTime();
    d.t.minutes = t.minutes;
    d.t.seconds = t.seconds;
    d.t.hours = t.hours;
}

void testState(data &d)
{
    char buffer[80 * 24];
    char output[] = "\033[38;5;%d;80;80mCurrent state : %d\n"
                    "Height: %0.2f\t|\tTemp: %0.2f\n"
                    "Solar_V: %0.2f\t|\tBatt_V: %0.2f\n"
                    "Uptime: %2d:%2d\n"
                    "Last Transmission time:%2d:%2d\n\033[0m";

    static int color = 16;
    color += 31; // comment this out if you hate fun :(

    sprintf(buffer, output,
            color, getState(),
            getHeight(), d.temp[0],
            d.powerData.solarPanelVoltage, d.powerData.batteryVoltage,
            d.t.minutes, d.t.seconds,
            0, 0);
    Serial.println(buffer);
}

#pragma endregion Debug

double checkPower(data &d)
{
    d.powerData.getData();               // update all power values;
    uartSwitch(RADIO, 9600, SERIAL_8N1); // in the event of failure, reconnect the radio;
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