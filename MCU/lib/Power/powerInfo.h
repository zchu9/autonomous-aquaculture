#ifndef POWERINFO_H
#define POWERINFO_H

// for accessing renogy MPPT
#include <renogy.h>

// for accessing victron SmartShunt
#include <victronFunctions.h>

// misc info.
#include "ArduinoJson.h"
#include "uartSwitch.h"

class powerInfo
{
public:
    double batteryVoltage;
    double solarPanelVoltage;

    RenogyMPPT mppt;
    ShuntPowerData bms;

    void initData();
    int updateData();
    int fetchVictronStats(ShuntPowerData& stats);
    double getBatteryVoltage();
    
    // debugging prints
    void printRenogyData();
    void printVictronData();
    void printVictronRawData();

private:
    bool checkFieldNum(size_t index);
    bool error(uint8_t index);
    void formatRenogyData();
    void formatVictronData();
    void hStatsVictron(uint8_t index);
};
#endif // POWERINFO_H