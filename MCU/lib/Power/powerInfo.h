#ifndef POWERINFO_H
#define POWERINFO_H

// for accessing renogy MPPT
#include <renogy.h>

// for accessing victron SmartShunt
#include <victron.h>
#include <victronFunctions.h>

// misc info.
#include <ArduinoJson.h>    // formatting


#include "uartSwitch.h"



class powerInfo {
public:
    double batteryVoltage;
    double solarPanelVoltage;

    RenogyMPPT mppt;
    ShuntPowerData bms;
    JsonDocument data;

    int getData();
    void formatRenogyData();
    void formatVictronData();

};
#endif // POWERINFO_H