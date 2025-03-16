#include "FSM.h"
#include "uartSwitch.h"

/**
 * @brief
 *
 */
void FSM(data &d)
{
    checkPowerHandler(d);
    commsHandler(d);
    // emergencyLiftHandler(d);
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
#else
    initializeLPMandNCM(d);
#endif
    // should probably be a init data function.
    d.liftFlag[0] = 0;
    // d.lowerFlag = 0;
    //  initialize the data struct
    d.power_placeholder = 0;
    // d.height = getHeight();
    memset(d.img, '\0', d.img_size);
    // d.whatever = "Nice data Zachary"; it is nice thank you everyone always says how nice it is, i didn't say that but everyone is saying its very very nice data, not like that nasty data some other poeple are bringing in here with its crime and all that
    // initializes the check power interrupt the comms handler, and the emergency lift lowering timer interrupt
    // initializeLPMandNCM(d);
}

/**
 * @brief Initializes variables to be used in the (Low Power/No Connection) Mode
 *
 */
void initializeLPMandNCM(data &d)
{
    // initialize: Low Power, No Connection, RF Mode
    d.state = LOW_POWER_NO_CONNECTION;

    /* initializeCheckPower();
    initializeReconnection();
    initializeEmergencyLiftLowering(); */

#if DEBUG
    Serial.println("Low Power and No Connection Mode Initialized");
#endif
}

/**
 * @brief Initialize the Low Power Mode\n
 * The LPM should be set to 0\n
 * check power should be initialized
 */
void initializeLPM(data &d)
{
    d.state = LOW_POWER;
}

/**
 * @brief Initialize the No Connection Mode\n
 * LPM should be set to 0\n
 * If NCM was 1, then the "reconnected" should be initialized already\n
 * TODO: initializeReconnection() is undefined
 */
void initializeNCM(data &d)
{
    d.state = NO_CONNECTION;
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
        checkPower(d);
        // d.power_placeholder = 100; // checkPower();
        powerStateChange(d);
    }
}

void commsHandler(data &d)
{
    if (getCommsFlag() == 1)
    {
#if DEBUG
        Serial.println("Comms Module Interrupt");
#endif
        setCommsFlag(false);
        bool ncm = d.state == NO_CONNECTION || d.state == LOW_POWER_NO_CONNECTION;
        if (ncm)
        {
            // attempt to reconnect
            // RFDisconnectedCase(d);
        }
        else
        {
            // listen for messages?
            RFConnectedCase(d);
        }
    }
}

/**
 * @brief Ideally The function of this code is:
 *  Try and receive message
 *      if received
 *          do the commands
 *      if not received
 *          throw error
 * TODO: THIS CODE HAS A BLOCKING LOOP THAT COULD GO ON FOREVER. ADD WDT
 *
 * @param d
 */
void RFConnectedCase(data &d)
{
#if DEBUG
    Serial.println("RF Connected");
#endif

    receiveMsg(d.doc);
    runCommands(d);
}

int runCommands(data &d)
{
    if (d.doc.isNull())
    {
        Serial.println("No valid JSON received");
        return -1;
    }
    if (d.doc["command"] == "1")
    {
        winchControl(d);
        Serial.println("Lift command received");
    }
    return 0;
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
};
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
    if (d.power_placeholder < POWER_THRESHOLD)
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

/////////////////////////////////////////
/**
 * @brief initialize serial communication
 */
void initializeDebug()
{
    Serial.begin(9600);
    while (!Serial)
        ;
    delay(100);
    Serial1.begin(9600, SERIAL_8N1);
    Serial.println("Debugging Initialized");
}
////////////////////////////////////////////////////////////////////////////
// Zach's House
////////////////////////////////////////////////////////////////////////////
double checkPower(data &d)
{
    double ret = messageTest(d);
    return ret;
}

void winchControl(data &d)
{
    unsigned long startTime = millis();
    unsigned long timeout = 5000;
    uint8_t index = -1;
    uint8_t numOfWinches = 4;

    for (uint8_t i = 0; i < numOfWinches; i++)
    {
        if (d.liftFlag[i])
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return; // oops
    }

    // check up/down from analogRead()
    // if high
    // index+numberofwinches

    // select winch
    digitalWrite(MUX_DISABLE_2, HIGH);
    digitalWrite(MUX_SEL_0, (index & 0x001));
    digitalWrite(MUX_SEL_1, (index & 0x010));
    digitalWrite(MUX_SEL_2, (index & 0x100)); // double check
    digitalWrite(MUX_DISABLE_2, LOW);

    while (millis() - startTime < timeout)
    {
        // check sensor
        // if (analogRead height) { stop if too high or low }
        // activate winch fires relay;
        digitalWrite(WINCH_ACTIVATE, HIGH);
    }
    digitalWrite(WINCH_ACTIVATE, LOW); // turn off
    d.liftFlag[index] = !d.liftFlag[index];
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