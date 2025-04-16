#ifndef POWERINFO_H
#define POWERINFO_H

// for accessing renogy MPPT
#include <renogy.h>

// for accessing victron SmartShunt
#include <victron.h>
#include <victronFunctions.h>

// misc info.
#include <ArduinoJson.h>    // formatting


class powerInfo {
public:
    RenogyMPPT mppt;
    ShuntPowerData bms;
    JsonDocument data;

    int getData();
    void formatRenogyData();
};
#endif // POWERINFO_H