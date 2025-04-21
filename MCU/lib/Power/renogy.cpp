#include "renogy.h"
#define DEBUG 1

RenogyMPPT::RenogyMPPT(int modbus_address)
{
    node.begin(modbus_address, Serial1);
    node.setTransmitBuffer(0, lowWord(0));  // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
    node.setTransmitBuffer(1, highWord(0)); // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
    delay(10);
    wrLoadControlMode(); // defaults to always on.
}

uint8_t RenogyMPPT::wrLoadControlMode(uint16_t mode)
{
    uint8_t result = node.writeSingleRegister(0x10A, mode);
    return result;
}

uint8_t RenogyMPPT::rdDataRegisters()
{
    uint8_t result;
    uint16_t dataRegisters[numDataRegisters];

    result = node.readHoldingRegisters(0x103, numDataRegisters);
    if (result == node.ku8MBSuccess)
    {

        this->renogyData.batterySoc = dataRegisters[0];
        this->renogyData.batteryVoltage = dataRegisters[1] * .1;
        this->renogyData.batteryChargingAmps = dataRegisters[2] * .1;

        this->renogyData.batteryChargingWatts = this->renogyData.batteryVoltage * this->renogyData.batteryChargingAmps;

        // 0x103 returns two bytes, one for battery and one for controller temp in c
        uint16_t raw_data = dataRegisters[3];                    // eg 5913
        this->renogyData.controllerTemperature = raw_data / 256; //* Maybe change to a bitmask for readability.
        this->renogyData.batteryTemperature = raw_data % 256;

        this->renogyData.controllerTemperatureF = (this->renogyData.controllerTemperature * 1.8) + 32;
        this->renogyData.batteryTemperatureF = (this->renogyData.batteryTemperature * 1.8) + 32;

        // load stats
        this->renogyData.loadVoltage = dataRegisters[4] * .1;
        this->renogyData.loadAmps = dataRegisters[5] * .01;
        this->renogyData.loadWatts = dataRegisters[6];

        // solar panel stats
        this->renogyData.solarPanelVoltage = dataRegisters[7] * .1;
        this->renogyData.solarPanelAmps = dataRegisters[8] * .01;
        this->renogyData.solarPanelWatts = dataRegisters[9];
        // data_registers[10] is a write only register

        // daily information
        this->renogyData.minBatteryVoltageToday = dataRegisters[11] * .1;
        this->renogyData.maxBatteryVoltageToday = dataRegisters[12] * .1;
        this->renogyData.maxChargingAmpsToday = dataRegisters[13] * .01;
        this->renogyData.maxDischargingAmpsToday = dataRegisters[14] * .1;

        this->renogyData.maxChargeWattsToday = dataRegisters[15];
        this->renogyData.maxDischargeWattsToday = dataRegisters[16];

        this->renogyData.chargeAmphoursToday = dataRegisters[17];
        this->renogyData.dischargeAmphoursToday = dataRegisters[18];

        this->renogyData.chargeWatthoursToday = dataRegisters[19];
        this->renogyData.dischargeWatthoursToday = dataRegisters[20];

        this->renogyData.controllerUptimeDays = dataRegisters[21];
        this->renogyData.totalBatteryOvercharges = dataRegisters[22];
        this->renogyData.totalBatteryFullcharges = dataRegisters[23];
        this->renogyData.lastUpdateTime = millis();
        /*
        Add these registers:
        Registers 0x118 to 0x119- Total Charging Amp-Hours - 24/25
        Registers 0x11A to 0x11B- Total Discharging Amp-Hours - 26/27
        Registers 0x11C to 0x11D- Total Cumulative power generation (kWH) - 28/29
        Registers 0x11E to 0x11F- Total Cumulative power consumption (kWH) - 30/31
        Register  0x120 - Load Status, Load Brightness, Charging State - 32
        Registers 0x121 to 0x122 - Controller fault codes - 33/34
        */
    }
#ifdef DEBUG
    else
    {
        if (result == 0xE2)
        {
            Serial.println("Timed out reading the data registers!");
        }
        else
        {
            Serial.print("Failed to read the data registers... ");
            Serial.println(result, HEX); // E2 is timeout
        }
    }
#endif
    return result;
}

uint8_t RenogyMPPT::rdInfoRegisters()
{
    int numInfoRegisters = this->numInfoRegisters;
    uint8_t result;
    uint16_t infoRegisters[numInfoRegisters];
    char buffer1[40], buffer2[40];
    uint8_t raw_data;

    result = node.readHoldingRegisters(0x00A, numInfoRegisters);
    if (result == node.ku8MBSuccess)
    {
        // read and process each value
        // Register 0x0A - Controller voltage and Current Rating - 0
        raw_data = infoRegisters[0];
        this->renogyInfo.voltageRating = raw_data / 255;
        this->renogyInfo.ampRating = raw_data % 255;
        this->renogyInfo.wattageRating = this->renogyInfo.voltageRating * this->renogyInfo.ampRating;

        // Register 0x0B - Controller discharge current and type - 1
        raw_data = infoRegisters[1];
        this->renogyInfo.dischargeAmpRating = raw_data / 256; // not sure if this should be /256 or /100
        this->renogyInfo.type = raw_data % 256;               // not sure if this should be /256 or /100

        // Registers 0x0C to 0x13 - Product Model String - 2-9

        // Registers 0x014 to 0x015 - Software Version - 10-11
        itoa(infoRegisters[10], buffer1, 10);
        itoa(infoRegisters[11], buffer2, 10);
        strcat(buffer1, buffer2);
        strcpy(this->renogyInfo.softwareVersion, buffer1);

        // Registers 0x016 to 0x017 - Hardware Version - 12-13
        itoa(infoRegisters[12], buffer1, 10);
        itoa(infoRegisters[13], buffer2, 10);
        strcat(buffer1, buffer2);
        strcpy(this->renogyInfo.hardwareVersion, buffer1);

        // Registers 0x018 to 0x019 - Product Serial Number - 14-15
        itoa(infoRegisters[14], buffer1, 10);
        itoa(infoRegisters[15], buffer2, 10);
        strcat(buffer1, buffer2);
        strcpy(this->renogyInfo.serialNumber, buffer1);

        this->renogyInfo.modbusAddress = infoRegisters[16];
        this->renogyInfo.lastUpdateTime = millis();
    }
#ifdef DEBUG
    else
    {
        if (result == 0xE2)
        {
            Serial.println("Timed out reading the info registers!");
        }
        else
        {
            Serial.print("Failed to read the info registers... ");
            Serial.println(result, HEX); // E2 is timeout
        }
    }
#endif
    return result;
};