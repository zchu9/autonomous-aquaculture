/**
 * @file powerInfo.cpp
 * @brief Handles the collection and formatting of data from the Victron SmartShunt and Renogy MPPT Solar controller.`
 * @version 0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <Arduino.h>
#include "powerInfo.h"
#include "timer.h"

bool powerInfo::checkFieldNum(size_t index)
{
    // Check if the field is a valid integer before converting
    if (this->bms.fields[index].empty() || !std::isdigit(this->bms.fields[index][0]) &&
                                               (this->bms.fields[index][0] != '-' || this->bms.fields[index].length() < 2 || !std::isdigit(this->bms.fields[index][1])))
    {
        Serial.print("Invalid integer value in field: ");
        Serial.println(this->bms.fields[index].c_str());
        return false;
    }
    return true;
}

void powerInfo::printVictronRawData()
{
    Serial.println("===== VICTRON RAW DATA =====");
    if (bms.labels.size() != bms.fields.size())
    {
        Serial.println("Error: Labels and fields size mismatch");
        return;
    }

    for (size_t i = 0; i < bms.labels.size(); i++)
    {
        Serial.print(bms.labels[i].c_str());
        Serial.print(": ");
        Serial.println(bms.fields[i].c_str());
    }
    Serial.println("===========================");
}

// stats come in two packets: one is primarily H##, the other is mixed.
void powerInfo::formatVictronData()
{

    // Find the shorter of the two lists to prevent out of bounds access
    size_t labelSize = bms.labels.size();
    size_t fieldsSize = bms.fields.size();
    size_t minSize = min(labelSize, fieldsSize);
    // Print the sizes of arrays for debugging
    Serial.print("Label size: ");
    Serial.print(labelSize);
    Serial.print(", Fields size: ");
    Serial.print(fieldsSize);
    Serial.print(", Using min size: ");
    Serial.println(minSize);

    for (size_t i = 0; i < minSize; i++)
    {
        Serial.print("Made it seeyuhhhh 1 ");
        Serial.println(i);

        if (!this->bms.labels[i].compare("Checksum"))
        {
            continue;
        }

        if (checkFieldNum(i))
        {
            if (this->bms.labels[i][0] == 'H')
            {
                Serial.print("Made it seeyuhhhh 2 ");
                Serial.println(i);
                hStatsVictron(i);
            }
            else
            {
                switch (this->bms.labels[i][0])
                {
                case 'V':
                    this->bms.mvoltage = stoi(this->bms.fields[i]);
                    break;
                case 'I':
                    this->bms.mcurrent = stoi(this->bms.fields[i]);
                    break;
                case 'P':
                    this->bms.power = stoi(this->bms.fields[i]);
                    break;
                case 'C':
                    this->bms.consumedmAH = stoi(this->bms.fields[i]);
                    break;
                case 'S':
                    this->bms.stateOfCharge = stoi(this->bms.fields[i]);
                    break;
                case 'T':
                    this->bms.timeToGo = stoi(this->bms.fields[i]);
                    break;
                case 'A':
                    if (this->bms.labels[i][1] == 'R')
                    {
                        this->bms.alarmReason = stoi(this->bms.fields[i]);
                    }
                case 'F':
                    this->bms.firmware = stoi(this->bms.fields[i]);
                    break;
                case 'M':
                    this->bms.monitorMode = stoi(this->bms.fields[i]);
                    break;

                default:
                    break;
                }
            }
        }
        else
        {
            switch (this->bms.labels[i][0])
            {
            case 'A':
                if (this->bms.labels[i][1] == 'R')
                {
                    break;
                }
                else
                {
                    this->bms.alarm = (this->bms.fields[i][1] == 'F') ? false : true;
                }
                break;
            case 'B':
                this->bms.model = this->bms.fields[i];
                break;
            default:
                break;
                // put the switch case with the ones that can take strings.
            }
            Serial.print("Made it seeyuhhhh 3 ");
            Serial.println(i);
        }
    }
}

// collect power data from each source, format it as needed.
// handles all serial line switching, as needed.
int powerInfo::updateData()
{
    // this should not take more than 60 seconds, timeout if needed.
    time t = getTime();
    // int timeoutM = (t.minutes + 1) % 60;
    int timeoutS = (t.seconds + 5) % 60;

    int successfulReads = 0;
    uint8_t ret;

    // Read from the SmartShunt
    uartSwitch(BMS, VICTRON_BAUD, VICTRON_CONFIG);
    while (successfulReads < 10)
    {
        ret = fetchVictronStats(this->bms);
        if (!ret)
        {
            this->printVictronRawData();
            this->formatVictronData();
            Serial.println("Made it seeyuhhhh");
            successfulReads++;
        }

        t = getTime();
        if (timeoutS == t.seconds)
        {
            successfulReads = 2; // timeout error
        }
    }

    // Read data from the MPPT
    uartSwitch(MPPT, RENOGY_BAUD, RENOGY_CONFIG);
    while (successfulReads < 11)
    {
        ret = this->mppt.rdDataRegisters();
        if (!error(ret))
        {
            // ! format
            successfulReads++;
        }
        t = getTime();
        Serial.print("MPPT1 att time: ");
        Serial.println(t.seconds);
        if (timeoutS == t.seconds)
        {
            return 1; // timeout error
        }
    }

    // Read info from the MPPT
    while (successfulReads < 12)
    {
        ret = this->mppt.rdInfoRegisters();
        if (!error(ret))
        {
            // ! format
            successfulReads++;
        }
        Serial.print("MPPT2 att time: ");
        Serial.println(t.seconds);
        t = getTime();
        if (timeoutS == t.seconds)
        {
            return 1; // timeout error
        }
    }

    return 0;
}

void powerInfo::hStatsVictron(uint8_t index)
{
    uint8_t value = stoi(this->bms.labels[index].substr(1));

#pragma region TODO

    switch (value)
    {
    case (1):
        this->bms.deepestDischargeDepth = stoi(this->bms.fields[index]);
        break;
    case (2):
        this->bms.lastDischargeDepth = stoi(this->bms.fields[index]);
        break;
    case (3):
        this->bms.avgDischargeDepth = stoi(this->bms.fields[index]);
        break;
    case (4):
        this->bms.chargeCycles = stoi(this->bms.fields[index]);
        break;
    case (5):
        this->bms.fullDischarges = stoi(this->bms.fields[index]);
        break;
    case (6):
        this->bms.totalAmpHoursDrawn = stoi(this->bms.fields[index]);
        break;
    case (7):
        this->bms.minMainBattVoltage = stoi(this->bms.fields[index]);
        break;
    case (8):
        this->bms.maxMainBattVoltage = stoi(this->bms.fields[index]);
        break;
    case (9):
        this->bms.secondsSinceLastFullCharge = stoi(this->bms.fields[index]);
        break;
    case (10):
        this->bms.numSynchros = stoi(this->bms.fields[index]);
        break;
    case (11):
        this->bms.numLowVoltAlarms = stoi(this->bms.fields[index]);

        break;
    case (12):
        this->bms.numHighVoltAlarms = stoi(this->bms.fields[index]);

        break;
    case (15):
        this->bms.auxBattMinimum = stoi(this->bms.fields[index]);

        break;
    case (16):
        this->bms.auxBattMaximum = stoi(this->bms.fields[index]);

        break;
    case (17):
        // Total Discharged Energy // DC MONITOR MODE
        break;
    case (18):
        // Total Charged Energy // DC MONITOR MODE
        break;
    default:
        break;
    }
}

bool powerInfo::error(uint8_t ret)
{
    char buf[60];
    if (!ret)
    {
        Serial.println("fuck");
        return true;
    }
    else
    {
        switch (ret)
        {
        case 1:
            sprintf(buf, "ret %2d: Timeout reading power data.", ret);
            break;
        case 2:
            sprintf(buf, "ret %2d: Misread Victron checksum.", ret);
            break;
        }
    }
    return false;
}

double powerInfo::getBatteryVoltage()
{
    this->batteryVoltage = this->mppt.renogyData.batteryVoltage;
    return this->batteryVoltage;
}

void powerInfo::printRenogyData()
{
    Serial.print("Battery Voltage: ");
    Serial.println(this->mppt.renogyData.batteryVoltage);
    Serial.print("Solar Panel Voltage: ");
    Serial.println(this->mppt.renogyData.solarPanelVoltage);
    Serial.print("Battery SOC: ");
    Serial.print(this->mppt.renogyData.batterySoc);
    Serial.println("%");
    Serial.print("Voltage Rating: ");
    Serial.println(this->mppt.renogyInfo.voltageRating);
    Serial.print("Amp Rating: ");
    Serial.println(this->mppt.renogyInfo.ampRating);
}

void powerInfo::printVictronData()
{
    Serial.println("===== VICTRON SMART SHUNT DATA =====");
    Serial.print("Battery Voltage: ");
    Serial.print(bms.mvoltage / 1000.0, 2);
    Serial.println(" V");

    Serial.print("Current: ");
    Serial.print(bms.mcurrent / 1000.0, 2);
    Serial.println(" A");

    Serial.print("Power: ");
    Serial.print(bms.power);
    Serial.println(" W");

    Serial.print("Consumed: ");
    Serial.print(bms.consumedmAH / 1000.0, 2);
    Serial.println(" Ah");

    Serial.print("State of Charge: ");
    Serial.print(bms.stateOfCharge);
    Serial.println("%");

    Serial.println("==============================");
}

void powerInfo::initData()
{
    // Initialize basic properties
    batteryVoltage = 255.0;
    solarPanelVoltage = 255.0;

    // Initialize Victron SmartShunt data
    clear(bms);
    bms.mvoltage = 255;
    bms.mcurrent = 255;
    bms.power = 255;
    bms.consumedmAH = 255;
    bms.stateOfCharge = 255;
    bms.timeToGo = 255;
    bms.alarm = false;
    bms.alarmReason = 255;
    bms.firmware = 255;
    bms.deepestDischargeDepth = 255;
    bms.lastDischargeDepth = 255;
    bms.avgDischargeDepth = 255;
    bms.chargeCycles = 255;
    bms.fullDischarges = 255;
    bms.totalAmpHoursDrawn = 255;
    bms.minMainBattVoltage = 255;
    bms.maxMainBattVoltage = 255;
    bms.secondsSinceLastFullCharge = 255;
    bms.numSynchros = 255;
    bms.numLowVoltAlarms = 255;
    bms.numHighVoltAlarms = 255;
    bms.auxBattMinimum = 255;
    bms.auxBattMaximum = 255;

    // Initialize string fields to empty strings
    bms.PID = "";
    bms.model = "";
    bms.monitorMode = "";

    // Clear any existing data in vectors
    bms.labels.clear();
    bms.fields.clear();

    // Initialize Renogy MPPT data
    mppt.renogyData.batterySoc = 255;
    mppt.renogyData.batteryVoltage = 255.0;
    mppt.renogyData.batteryChargingAmps = 255.0;
    mppt.renogyData.batteryTemperature = 255;
    mppt.renogyData.controllerTemperature = 255;
    mppt.renogyData.loadVoltage = 255.0;
    mppt.renogyData.loadAmps = 255.0;
    mppt.renogyData.loadWatts = 255;
    mppt.renogyData.solarPanelVoltage = 255.0;
    mppt.renogyData.solarPanelAmps = 255.0;
    mppt.renogyData.solarPanelWatts = 255;
    mppt.renogyData.minBatteryVoltageToday = 255.0;
    mppt.renogyData.maxBatteryVoltageToday = 255.0;
    mppt.renogyData.maxChargingAmpsToday = 255.0;
    mppt.renogyData.maxDischargingAmpsToday = 255.0;
    mppt.renogyData.maxChargeWattsToday = 255;
    mppt.renogyData.maxDischargeWattsToday = 255;
    mppt.renogyData.chargeAmphoursToday = 255;
    mppt.renogyData.dischargeAmphoursToday = 255;
    mppt.renogyData.chargeWatthoursToday = 255;
    mppt.renogyData.dischargeWatthoursToday = 255;
    mppt.renogyData.controllerUptimeDays = 255;
    mppt.renogyData.totalBatteryOvercharges = 255;
    mppt.renogyData.totalBatteryFullcharges = 255;
    mppt.renogyData.batteryTemperatureF = 255.0;
    mppt.renogyData.controllerTemperatureF = 255.0;
    mppt.renogyData.batteryChargingWatts = 255.0;
    mppt.renogyData.lastUpdateTime = 0;
    mppt.renogyData.controllerConnected = false;

    // Initialize controller info
    mppt.renogyInfo.voltageRating = 255;
    mppt.renogyInfo.ampRating = 255;
    mppt.renogyInfo.dischargeAmpRating = 255;
    mppt.renogyInfo.type = 255;
    mppt.renogyInfo.controllerName = 255;
    mppt.renogyInfo.wattageRating = 255.0;
    mppt.renogyInfo.lastUpdateTime = 0;
    mppt.renogyInfo.modbusAddress = 255;

    // Clear string fields
    memset(mppt.renogyInfo.softwareVersion, 0, sizeof(mppt.renogyInfo.softwareVersion));
    memset(mppt.renogyInfo.hardwareVersion, 0, sizeof(mppt.renogyInfo.hardwareVersion));
    memset(mppt.renogyInfo.serialNumber, 0, sizeof(mppt.renogyInfo.serialNumber));

    // Clear JSON document
    data.clear();
}