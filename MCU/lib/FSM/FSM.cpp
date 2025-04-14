#include "FSM.h"
#include "uartSwitch.h"

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
    //  emergencyLiftHandler(d);
}

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
    uartSwitch(d, RADIO, 9600, SERIAL_8N1);
    // should probably be a init data function.

    // init the lora class
    d.lora = new LoraRadio;
    d.winch = new winchData(LIFT_PIN, LOWER_PIN, A0);
    d.liftFlag[0] = 0;
    // d.lowerFlag = 0;
    //  initialize the data struct

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
        // checkPower(d);
    }
}

//////////////////////////////////////////
//          Comms stuff
//////////////////////////////////////////

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

bool sendImage(data &d)
{
}

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

void getImg(data &d)
{
    
}

/*
Shiz is lowk deprecated but I like the way it looks.
void getImgSeg()
{
    const int chunksize = 250;
    char buffer[chunksize] = {'\0'};

    // empty fifo buffer
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();

    uint32_t imgSize = myCAM.read_fifo_length();

    int chunk = chunksize;

    // send chunk -> update size -> repeat until empty
    while (imgSize > 0)
    {
        memset(buffer, '\0', chunksize);
        if (imgSize < chunksize)
        {
            chunk = imgSize;
        }
        getPartialImage(buffer, chunk);
        // at this point, buffer contains a bit of the image
        imgSize -= chunk;
    }
    myCAM.clear_fifo_flag();
}
    */

/**
 * @brief initialize serial communication
 */
void initializeDebug()
{
    Serial.begin(9600);
    // This is blocking until a serial monitor is connected
    // while (!Serial)
    // ;
    // delay(100);
    Serial1.begin(9600, SERIAL_8N1);
    Serial.println("Debugging Initialized");
}
////////////////////////////////////////////////////////////////////////////
// Zach's House
////////////////////////////////////////////////////////////////////////////
double checkPower(data &d)
{
    // double ret = messageTest(d);
    return 0;
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