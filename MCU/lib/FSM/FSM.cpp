#include "FSM.h"

static int commsFlag = 0;
static bool lowPowerMode = false;
static bool noConnectionMode = false;

void FSM(data &d)
{
    checkPowerHandler(d);
    loraListen(d);
    testState(d);
}

#pragma region Initialize

/**
 * @brief initializes the system at startup. The system should be put into low power mode and no connection mode.\n
 * The data struct should be initialized\n
 * The power check system should be initialized\n
 * The comms establish connection Protocol should be initialized\n
 * The emergency lift lowering protocol should be initialized\n
 *
 * @param d the data struct that will be passed from the main function.
 */
void initializeStartup(data &d)
{
#if DEBUG
    initializeDebug();
#endif

    attachInterrupt(digitalPinToInterrupt(RX_INTERRUPT), commsHandler, RISING);
    //  init the heap pointers (jayson said that we should do this)
    Serial1.begin(9600, SERIAL_8N1);
    timerInit();
    initMuxPins();
    d.powerData = new powerInfo;
    d.powerData->initData();
    d.lora = new LoraRadio;
    d.winch = new winchData(LIFT_PIN, LOWER_PIN, POT_PIN);

    d.cam->begin();
    // noConnectionMode = d.lora->sendHandshake();
}

/**
 * @brief initialize serial communication
 */
void initializeDebug()
{
    Serial.begin(9600);
    Serial.println("Debugging Initialized");
}

#pragma endregion Initialize

#pragma region Power

void checkPowerHandler(data &d)
{
    if (getPowerFlag() == 1)
    {
#if DEBUG
        Serial.println("Power check function");
#endif
        setPowerFlag(false);

        d.powerData->updateData(); // fetch new data from the controllers;

        double battVoltage = d.powerData->getBatteryVoltage();

        if (battVoltage < POWER_THRESHOLD)
        {
            lowPowerMode = true;
            return;
        }

        if (battVoltage >= POWER_THRESHOLD)
        {
            lowPowerMode = false;
        }
        if ((noConnectionMode = sendData(d)))
        {
            // TODO: do something dependent on if the data was sent or not.
        }
    }
}

#pragma endregion Power

//////////////////////////////////////////
//          Comms stuff
//////////////////////////////////////////

#pragma region Comms

void commsHandler()
{
    if (commsFlag == 0)
    {
        commsFlag = 1;
    }
}

void loraListen(data &d)
{
    if (commsFlag == 1)
    {
#if DEBUG
        Serial.println("Comms Module Interrupt");
#endif
        detachInterrupt(digitalPinToInterrupt(RX_INTERRUPT));
        d.lora->receiveMsg(d.doc);
        runCommands(d);
        commsFlag = 0;
        attachInterrupt(digitalPinToInterrupt(RX_INTERRUPT), commsHandler, RISING);
    }
}

int runCommands(data &d)
{
    if (d.doc.isNull())
    {
        Serial.println("No valid JSON received");
        return -1;
    }
    if (d.doc["command"] == 1)
    {
        if (lowPowerMode == true)
        {
            sendError(d);
        }
        else
        {
            d.winch->lift(3.3); // TODO: Hard-coded values
        }
        Serial.println(">>>>>Lift command received");
        getAndSendImg(d);
    }
    if (d.doc["command"] == 0)
    {
        if (lowPowerMode == true)
        {
            sendError(d);
        }
        else
        {
            d.winch->lower(0); // TODO: Hard-coded values
        }
        Serial.println(">>>>>Lower command received");
        getAndSendImg(d);
    }
    // clear the json doc
    d.doc.clear();
    return 0;
}

bool sendData(data &d)
{
    // Convert the data struct to JSON
    JsonDocument doc = jsonify(d);
    if (doc.isNull())
    {
        Serial.print("syuh");
        return false;
    }

    size_t len = measureJson(doc);
    char *buffer = new char[len + 1]; // +1 for null terminator
    serializeJson(doc, buffer, len + 1);
    // Send the JSON over LoRa
    bool success = d.lora->sendPackets(buffer);
    if (success)
    {
        // TODO: clear the data struct
    }
    delete[] buffer; // Free the allocated memory
    return success;
}

bool sendError(data &d)
{
    // Convert the data struct to JSON
    JsonDocument doc;
    doc["state"] = "Winch Operation Error: Low Power Mode";
    size_t len = measureJson(doc);
    char *buffer = new char[len + 1]; // +1 for null terminator
    serializeJson(doc, buffer, len + 1);
    // Send the JSON over LoRa
    bool success = d.lora->sendPackets(buffer);
    Serial.print("made it out 100"); // TODO: Remove later
    delete[] buffer;                 // Free the allocated memory
    return success;
}

#pragma endregion Comms

#pragma region Sensors

bool getAndSendImg(data &d)
{
    uint32_t imgSize = d.cam->captureImage();
    // uint32_t imgSize = d.cam->myCAM->read_fifo_length();

    Serial.print("Image size: ");
    Serial.println(imgSize);

    uint32_t imDivider = 6;
    int chunkSize = imgSize / imDivider;
    uint8_t img[chunkSize];

    int numBytesRead = 1;
    int currentChunk = 0;

    if (imgSize > 0)
    {
        d.cam->startImageStream(); // this may cause the beginning to repeat.

        while (numBytesRead > 0)
        {
            currentChunk++;

            numBytesRead = d.cam->readImageChunk(chunkSize, img);

            int encodedLength = Base64.encodedLength(chunkSize);
            char encodedImg[encodedLength + 1];

            Base64.encode(encodedImg, (char *)img, chunkSize);

            if (d.lora->sendPackets(encodedImg))
            {
                Serial.println("Chunk:" + String(currentChunk + 1) + " sent");
            }
            else
            {
                Serial.println("Failed to send chunk:" + String(currentChunk + 1));
                d.cam->finishImageStream(); // returns CS line high on failure.
                return false;
            }
        }
    }
    else
    {
        Serial.println("Failed to capture image.");
        d.cam->finishImageStream();
        return false;
    }

    d.cam->finishImageStream();
    return true;
}

void updateTemp(data &d)
{
    // Read temperature
    // float currentTempC = getTempC();
    d.temp = getTempF();
}

#pragma endregion Sensors

#pragma region Helpers
JsonDocument jsonify(data &d)
{
    JsonDocument doc;
    doc.clear(); // Clear previous data

    // Create a nested JsonObject for power-related data
    JsonObject power = doc["power"].to<JsonObject>();
    // Create a nested JsonArray for smart_shunt data inside power object
    JsonArray smart_shunt = power["smart_shunt"].to<JsonArray>();
    // Create a nested JsonObject as the first element of smart_shunt array
    JsonObject shunt_data = smart_shunt.add<JsonObject>();
    // Populate Victron SmartShunt data from powerData
    shunt_data["battery_voltage"] = d.powerData->bms.mvoltage / 1000.0; // Convert mV to V
    shunt_data["battery_current"] = d.powerData->bms.mcurrent / 1000.0; // Convert mA to A
    shunt_data["power"] = d.powerData->bms.power;
    shunt_data["state_of_charge"] = d.powerData->bms.stateOfCharge;
    shunt_data["time_to_go"] = d.powerData->bms.timeToGo;
    shunt_data["alarm_status"] = d.powerData->bms.alarm;
    shunt_data["alarm_reason"] = d.powerData->bms.alarmReason;
    shunt_data["firmware_version"] = d.powerData->bms.firmware;
    shunt_data["model_description"] = d.powerData->bms.model;

    // Add historical data
    shunt_data["deepest_discharged"] = d.powerData->bms.deepestDischargeDepth;
    shunt_data["last_discharge"] = d.powerData->bms.lastDischargeDepth;
    shunt_data["avg_discharge"] = d.powerData->bms.avgDischargeDepth;
    shunt_data["charge_cycles"] = d.powerData->bms.chargeCycles;
    shunt_data["full_discharge_cycles"] = d.powerData->bms.fullDischarges;
    shunt_data["cum_amp_hrs_drawn"] = d.powerData->bms.totalAmpHoursDrawn;
    shunt_data["min_volt_battery"] = d.powerData->bms.minMainBattVoltage / 1000.0;
    shunt_data["max_volt_battery"] = d.powerData->bms.maxMainBattVoltage / 1000.0;
    shunt_data["last_full_charge"] = d.powerData->bms.secondsSinceLastFullCharge;
    shunt_data["auto_sync_count"] = d.powerData->bms.numSynchros;
    shunt_data["low_volt_alarm_count"] = d.powerData->bms.numLowVoltAlarms;
    shunt_data["high_volt_alarm_count"] = d.powerData->bms.numHighVoltAlarms;

    /*
    // Add Renogy MPPT data
    JsonObject mpptData = data.createNestedObject("renogy_mppt");
    mpptData["battery_voltage"] = mppt.renogyData.batteryVoltage;
    mpptData["solar_voltage"] = mppt.renogyData.solarPanelVoltage;
    mpptData["battery_soc"] = mppt.renogyData.batterySoc;
    mpptData["battery_charging_amps"] = mppt.renogyData.batteryChargingAmps;
    mpptData["battery_charging_watts"] = mppt.renogyData.batteryChargingWatts;
    mpptData["battery_temp"] = mppt.renogyData.batteryTemperature;
    mpptData["controller_temp"] = mppt.renogyData.controllerTemperature;
    mpptData["controller_connected"] = mppt.renogyData.controllerConnected;
    */
    JsonObject sensor = doc["sensor"].to<JsonObject>();
    sensor["state"] = getState();
    sensor["height"] = getHeight();
    sensor["temperature"] = getTempF();

    return doc;
}

std::string getState()
{
    if (lowPowerMode && noConnectionMode)
    {
        return "Low Power No Connection";
    }
    else if (lowPowerMode)
    {
        return "Low Power";
    }
    else if (noConnectionMode)
    {
        return "No Connection";
    }
    else
    {
        return "Normal";
    }
}

#pragma endregion Helpers

////////////////////////////////////////////////////////////////////////////
// Zach's House
////////////////////////////////////////////////////////////////////////////

#pragma region Debug

void updateTime(data &d)
{
    time t = getTime();
    d.t.minutes = t.minutes;
    d.t.seconds = t.seconds;
    d.t.hours = t.hours;
}

void testState(data &d)
{
    static int s = (d.t.seconds + 10) % 60;
    updateTime(d);
    if (d.t.seconds == s)
    {
        s = (d.t.seconds + 10) % 60;
        char buffer[80 * 24];
        char output[] = "\033[38;5;%d;80;80mCurrent state : %s\n"
                        "Height: %0.2f\t|\tTemp: %0.2f\n"
                        "Solar_V: %2.2f\t|\tBatt_V: %2.2f\n"
                        "Uptime: %2d:%2d\n"
                        "Last Transmission time:%2d:%2d\n\033[0m";

        static int color = 16;
        color += 31; // comment this out if you hate fun :(

        sprintf(buffer, output,
                color, getState().c_str(),
                getHeight(), d.temp,
                d.powerData->solarPanelVoltage, d.powerData->batteryVoltage,
                d.t.minutes, d.t.seconds,
                0, 0);
        Serial.println(buffer);
    }
}

#pragma endregion Debug

/*
      `'::::.
        _____A_
       /      /\
    __/__/\__/  \___
---/__|" '' "| /___/\----
   |''|"'||'"| |' '||
   `""`""))""`"`""""`
*/