#include <vector>
#include <string>

#define CRETURN '\r' // 0x0D, 0x0A
#define NEWLINE '\n'
#define TAB '\t'        // 0x09
#define VICTRON_BAUD 19200
#define VICTRON_CONFIG SERIAL_8N1

#ifndef VICTRONF_H
#define VICTRONF_H

// labels can be found in documentation.
// TODO: only return data of interest
struct ShuntPowerData {
    std::vector<std::string> labels;
    std::vector<std::string> fields;

    std::string PID;
    
    int mvoltage;
    int mcurrent;
    int power;
    int consumedmAH;
    int stateOfCharge;  // %

    int timeToGo; // minutes
    bool alarm;
    int alarmReason;
    std::string model;
    int firmware;

    std::string monitorMode;    // unused.
    int deepestDischargeDepth;
    int lastDischargeDepth;
    int avgDischargeDepth;
    int chargeCycles;

    int fullDischarges;
    int totalAmpHoursDrawn;
    int minMainBattVoltage;
    int maxMainBattVoltage;
    int secondsSinceLastFullCharge;
    
    int numSynchros;
    int numLowVoltAlarms;
    int numHighVoltAlarms;
    int auxBattMinimum;
    int auxBattMaximum;

    // unused; dc monitor mode stats.
    // H17
    // H18
};

bool victronChecksum(uint8_t* serialData, size_t dataSize);
void clear(ShuntPowerData& data);
void victronParse(ShuntPowerData& data, uint8_t* buffer, size_t buffer_size);

#endif // VICTRONF_H