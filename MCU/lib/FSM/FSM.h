#pragma once

#ifndef FSM_H
#define FSM_H

#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
#include "pins.h"
#include <Base64.h>
#include "LoRa.h"
#include "Sensors/camera_handler.h"
#include "Sensors/pot_handler.h"
#include "Sensors/temperature_sensor.h"
#include "winch.h"
#include "powerInfo.h" // for all power information
#include "timer.h"
#include "uartSwitch.h"

#define DEBUG true

struct data
{
    // Assuming the size of the array is 10, you can adjust it as needed
    static const size_t numWinches = 10;

    LoraRadio *lora; // a class to hold all the lora variables.
    winchData *winch;
    CameraHandler *cam;

    double temp; // stores temperature readings for each device

    JsonDocument doc;

    powerInfo *powerData; // stores power readings for each device
    time t;               // a value in minutes and seconds; need a better representation maybe.
    byte last;
};

#define POWER_THRESHOLD 11.8
#define MAX_HEIGHT 3.5

// Emergency Lift Lowering
#define TIME_UNTIL_EMERGENCY_LIFT_LOWER_HRS 4

// main function
void FSM(data &d);

// initialization functions
void initializeStartup(data &d);
/**
 * @brief
 *
 * @param d
 */
void checkPowerHandler(data &d);
/**
 * @brief Calls the lora recieve message function given the comms interrupt hits
 *
 * @param d
 */
void loraListen(data &d);

/**
 * @brief Called when the device is in low power mode and no connection mode for 6 hours.n
 *
 * @param d
 */
void emergencyLiftHandler(data &d);

// power helpers
/**
 * @brief updates the power data struct with the current power readings.\n
 *
 * @param d
 * @return double The current battery voltage
 */
double checkPower(data &d);

/**
 * @brief Called on serial edge Rising interrupt
 *
 * @param d data struct
 */
void commsHandler();

bool sendData(data &d);
bool sendError(data &d);

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

/**
 * @brief Get the State object
 *
 * @return std::string
 */
std::string getState();

// Sensor Controls
bool getAndSendImg(data &d);

// Temp sensor
void updateTemp(data &d);

// DEBUG CONTROLS

/**
 * @brief a function to test the FSM
 *
 * @param d FSM data struct
 * @param ds decide whether to simulate values in place of missing systems. values are power and sensors.
 * @param params in the form p=#
 *      h - height
 *      t - temp
 *      b - battery
 *      s = solar PV
 */
void testState(data &d);

void updateTime(data &d);

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