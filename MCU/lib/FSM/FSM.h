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
#include <WDTZero.h>

#define DEBUG true

enum farmState { STANDARD, LOW_POWER, NO_CONNECTION };

struct data
{
    // Assuming the size of the array is 10, you can adjust it as needed
    static const size_t numWinches = 10;

    LoraRadio *lora; // a class to hold all the lora variables.
    winchData *winch;
    CameraHandler *cam;
    WDTZero *watch;
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
 * @brief Print data related to the current state.
 *
 * @param d FSM data struct
 *
 */
void testState(data &d);

void updateTime(data &d);

// Debugging; initializes USB serial.
void initializeDebug();

#endif // FSM_H