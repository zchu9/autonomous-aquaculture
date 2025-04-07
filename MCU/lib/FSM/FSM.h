#ifndef FSM_H
#define FSM_H
#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
#include "pins.h" 
#include <Base64.h>
#include <Wire.h>
#include "interrupts.h"
#include "LoRa.h"
#include "ArduinoJson.h"
#include "Sensors/camera_handler.h" 
#include "Sensors/pot_handler.h"    
#include "Sensors/temperature_sensor.h" 

#define DEBUG true

enum State
{
    NORMAL,
    LOW_POWER,
    NO_CONNECTION,
    LOW_POWER_NO_CONNECTION
};

enum device
{
    BMS,
    MPPT,
    RADIO,
}; // victron bms, renogy mppt, LoRA module.

struct data
{
    // Assuming the size of the array is 10, you can adjust it as needed
    static const size_t numWinches = 10;

    bool liftFlag[numWinches];
    bool liftStarted;
    // bool lowerFlag[numWinches];
    State state;

    double height;
    std::vector<double> power; // stores power readings for each device
    std::vector<double> temp;  // stores temperature readings for each device
    uint8_t *img;

    device currentDevice;
    ulong lastDevSwitchTime;
    JsonDocument doc;
};

#define POWER_THRESHOLD 0.5

// Emergency Lift Lowering
#define TIME_UNTIL_EMERGENCY_LIFT_LOWER_HRS 4

// main function
void FSM(data &d);

// initialization functions
void initializeStartup(data &d);
void initializeNormalFSM(data &d);
void initializeLPM(data &d);
void initializeNCM(data &d);
void initializeLPMandNCM(data &d);
// interrupt initialization functions
void initializeCheckPower();
void initializeReconnection();
void initializeEmergencyLiftLowering();

// Main Loop
void sleep();
void checkPowerHandler(data &d);
void commsHandler(data &d);
void emergencyLiftHandler(data &d);

// power helpers
double checkPower(data &d);

void getIntoLowPowerMode(data &d);
void getOutOfLowPowerMode(data &d);

void powerStateChange(data &d);

// comms helpers
void RFConnectedCase(data &d);
/**
 * @brief TODO: Disconnected state will occur when the device fails to recieve a transmission within
 * SIGN_OF_LIFE_TIMEOUT.\n
 *
 * @param d
 */
void RFDisconnectedCase(data &d);

bool sendData(data &d);

/**
 * @brief Reads Json and runs the commands in the json doc.\n
 * Should probably clear the JSON doc after running the commands.\n
 *
 * TODO: Only LIFT is coded rn
 * @param doc
 * @return int returns 0 if the command is successfully run.\n
 */
int runCommands(data &d);

JsonDocument jsonify(data &d);

bool sendImage(data &d);

// Logan functions

// Sensor Controls
void getImg(data &d);


// Winch Controls

void winchControl(data &d);

// Debugging
void initializeDebug();

/**
 * @brief No longer applicable.
 *
 * @param d
 */
void CellularConnectedCase(data &d);
void CellularDisconnectedCase(data &d);

#endif // FSM_H