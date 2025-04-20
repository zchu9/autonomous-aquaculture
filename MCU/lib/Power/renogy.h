#ifndef RENOGY_H
#define RENOGY_H

// functional modbus implementation. very messy. replaces previous library which filled 0s to failed reads.

#include <Arduino.h>
#include <ModbusMaster.h>

#define RENOGY_BAUD 9600
#define RENOGY_CONFIG SERIAL_8N1    // although the modbusRTU spec differs, their implementation is simple.

// A struct to hold the controller data
struct controllerData {
    uint8_t batterySoc;               // percent
    float batteryVoltage;             // volts
    float batteryChargingAmps;       // amps

    uint8_t batteryTemperature;       // celcius
    uint8_t controllerTemperature;    // celcius

    float loadVoltage;                // volts
    float loadAmps;                   // amps
    uint8_t loadWatts;                // watts

    float solarPanelVoltage;         // volts
    float solarPanelAmps;            // amps
    uint8_t solarPanelWatts;         // watts

    float minBatteryVoltageToday;   // volts
    float maxBatteryVoltageToday;   // volts
    float maxChargingAmpsToday;     // amps
    float maxDischargingAmpsToday;  // amps

    uint8_t maxChargeWattsToday;    // watts
    uint8_t maxDischargeWattsToday; // watts
    uint8_t chargeAmphoursToday;     // amp hours
    uint8_t dischargeAmphoursToday;  // amp hours

    uint8_t chargeWatthoursToday;    // watt hours
    uint8_t dischargeWatthoursToday; // watt hours

    uint8_t controllerUptimeDays;    // days
    uint8_t totalBatteryOvercharges; // count
    uint8_t totalBatteryFullcharges; // count

    // convenience values
    float batteryTemperatureF;        // fahrenheit
    float controllerTemperatureF;     // fahrenheit
    float batteryChargingWatts;      // watts. 
    long lastUpdateTime;             // millis() of last update time
    bool controllerConnected;         // bool if we successfully read data from the controller
};

// A struct to hold the controller info params
struct controllerInfo {
    uint8_t voltageRating;            // volts
    uint8_t ampRating;                // amps
    uint8_t dischargeAmpRating;      // amps
    uint8_t type;
    uint8_t controllerName;
    char softwareVersion[40];
    char hardwareVersion[40];
    char serialNumber[40];
    uint8_t modbusAddress;

    float wattageRating;
    long lastUpdateTime;           // millis() of last update time
};

class RenogyMPPT {
public:
    RenogyMPPT(int modbusAddress = 255);
    uint8_t rdDataRegisters();
    uint8_t rdInfoRegisters();

    /**
     * @brief Writes the load control mode for the Renogy Rover MPPT.
     * @param mode The working modes for the Rover can be found in the Modbus documentation. Useful values are 0x0 (on when the solar panel is on) and 0xF1 (Always on).
     *
     * @return Returns the Modbus error code. 0xE2 is the most common; Timeout generally means the device is disconnected.
     */
    uint8_t wrLoadControlMode(uint16_t mode = 0xF1);
    
    controllerData renogyData;
    controllerInfo renogyInfo;
private:
    ModbusMaster node;
    const uint32_t numDataRegisters = 35; // registers are read sequentially from 0x103 on the Rover EEPROM.
    const uint32_t numInfoRegisters = 17; // info registers are from 0x00A. These contain device info.
};

#endif // RENOGY_H