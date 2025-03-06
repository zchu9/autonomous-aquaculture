#include "FSM.h"

/**
 * @brief
 *
 */
void FSM(data &d)
{
    sleep();
    // Serial.println("sleep dont work");
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
#endif
    // should probably be a init data function.
    d.liftFlag = 0;
    d.lowerFlag = 0;
    // initialize the data struct
    d.power_placeholder = 0;
    // d.height = getHeight();
    d.img[10] = {0};
    d.whatever = "Nice data Zachary";
    // initializes the check power interrupt the comms handler, and the emergency lift lowering timer interrupt
    initializeLPMandNCM(d);
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
        d.power_placeholder = 100; // checkPower();
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
            RFDisconnectedCase(d);
        }
        else
        {
            // listen for messages?
            RFConnectedCase(d);
        }
    }
}

void RFDisconnectedCase(data &d)
{
#if DEBUG
    Serial.println("RF Disconnected");
#endif
}

void RFConnectedCase(data &d)
{
#if DEBUG
    Serial.println("RF Connected");
#endif
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
    pinMode(13, INPUT);
    Serial.println("Debugging Initialized");
}
////////////////////////////////////////////////////////////////////////////
// Zach's Room
////////////////////////////////////////////////////////////////////////////
double checkPower(data &d)
{
#if DEBUG
    if (Serial1.available())
    {
        messageTest(d);
    }
#endif
    return 0.0;
}

void uartSwitch(device d, long baud, uint16_t config)
{

    // disable the mux, select new lines.
#ifndef DEBUG
    digitalWrite(MUX_DISABLE, HIGH);
    digitalWrite(MUX_PIN_0, (d & 0x01));
    digitalWrite(MUX_PIN_1, (d & 0x10));
#endif

    // assert(SERCOM5->USART.CTRLB.bit.CHSIZE == 0);  // check 8 bits is configured

    Serial1.flush();             // Wait for tx to clear.
    Serial1.begin(baud, config); // change baud.
    while (Serial1.available())
    {
        Serial1.read(); // clear input buffer.
    }

#ifndef DEBUG
    digitalWrite(MUX_DISABLE, LOW);
#endif
}

#ifdef DEBUG
int messageTest(data &data)
{
    // we anticipate the data sent from the power systems will be in a regular format
    uartSwitch(MPPT, 19200, SERIAL_8N1);
    String label;
    String field;
    size_t buffer_size = 6;
    byte buffer[buffer_size];

    // victron types first;
    // we expect a label, tab, field, and lastly \r\n
    while (Serial1.available())
    {
        label = Serial1.readStringUntil('\t');
        field = Serial1.readStringUntil('\r');
    }

    if (!label.compareTo("V"))
    {
        data.whatever = field; // if the field matches, store it.
    }

    // TODO: implement a switch mechanism or wait here.

    // secondly renogy types.
    // first reconfigure uart.
    uartSwitch(MPPT, 9600, SERIAL_8E1);
    // we expect address, function, 2 bytes data, and 2 bytes crc.
    // ignoring all except data for simplicity.
    while (Serial1.available())
    {

        Serial1.readBytes(buffer, buffer_size);
    }
    byte value = (buffer[3] << 8) | (buffer[4]); // data transmitted is high byte then low byte.

    data.whatever = String(value);

    Serial.println(data.whatever);
    // TODO: add a radio segment to change the line to the LoRA module.
    uartSwitch(MPPT, 9600, SERIAL_8N1);
    return 1;
}
#endif