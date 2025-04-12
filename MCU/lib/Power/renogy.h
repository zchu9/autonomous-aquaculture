// functional modbus implementation. very messy. replaces previous library which filled 0s to failed reads.

#include <Arduino.h>
#include <renogyFunctions.h>
#include <ModbusMaster.h>

void getDeviceAddress() {
    // modbus devices have an address between 1 and 256; 0 is global broadcast.
    // polling all devices may be necessary to find our device. this function is time consuming.
    uint8_t startAddress = 0x01;
    uint8_t endAddress = 0xFF;

    // modbus message; attempts to read system voltage/current at 0x000A.
    const size_t message_size = 8;
    uint8_t message[message_size] = { '\0', READREG, 0, 0xA, 0, 1, '\0', '\0' };
    char serialMessage[message_size];

    // TODO: configure UART
    // then send to each address and check for a response.
    for (uint8_t i = startAddress; i < endAddress; i++) {
        message[0] = i; // address
        int crc = genCRC(message, 6);
        message[6] = crc & 0xFF;    // crc low
        message[7] = (crc >> 8) & 0xFF; // crc high
        memcpy(serialMessage, message, sizeof(uint8_t));    // u_char cannot be cast to char; this avoids cast errors.
        while (Serial1.available() > 0) {
            Serial1.read(); // clear input buffer.
        }
        Serial1.write(serialMessage);

        delay(RENOGY_DELAY);    // some arbitrary delay to ensure a response from the device.

        if (Serial1.available() > 0) {
            // TODO: if there's a response, check it's validity then return address i;
            // otherwise move to the next address.
        }

    }
};

// A struct to hold the controller data
struct Controller_data {

    uint8_t battery_soc;               // percent
    float battery_voltage;             // volts
    float battery_charging_amps;       // amps
    uint8_t battery_temperature;       // celcius
    uint8_t controller_temperature;    // celcius
    float load_voltage;                // volts
    float load_amps;                   // amps
    uint8_t load_watts;                // watts
    float solar_panel_voltage;         // volts
    float solar_panel_amps;            // amps
    uint8_t solar_panel_watts;         // watts
    float min_battery_voltage_today;   // volts
    float max_battery_voltage_today;   // volts
    float max_charging_amps_today;     // amps
    float max_discharging_amps_today;  // amps
    uint8_t max_charge_watts_today;    // watts
    uint8_t max_discharge_watts_today; // watts
    uint8_t charge_amphours_today;     // amp hours
    uint8_t discharge_amphours_today;  // amp hours
    uint8_t charge_watthours_today;    // watt hours
    uint8_t discharge_watthours_today; // watt hours
    uint8_t controller_uptime_days;    // days
    uint8_t total_battery_overcharges; // count
    uint8_t total_battery_fullcharges; // count

    // convenience values
    float battery_temperatureF;        // fahrenheit
    float controller_temperatureF;     // fahrenheit
    float battery_charging_watts;      // watts. 
    long last_update_time;             // millis() of last update time
    bool controller_connected;         // bool if we successfully read data from the controller
};

// A struct to hold the controller info params
struct Controller_info {

    uint8_t voltage_rating;            // volts
    uint8_t amp_rating;                // amps
    uint8_t discharge_amp_rating;      // amps
    uint8_t type;
    uint8_t controller_name;
    char software_version[40];
    char hardware_version[40];
    char serial_number[40];
    uint8_t modbus_address;

    float wattage_rating;
    long last_update_time;           // millis() of last update time
};

class RenogyMPPT {
public:
    RenogyMPPT(int modbus_address = 255);
    void renogy_read_data_registers();
    void renogy_read_info_registers();

private:
    ModbusMaster node;

    /*
    Number of registers to check. I think all Renogy controllers have 35
    data registers (not all of which are used) and 17 info registers.
    */
    Controller_data renogyData;
    Controller_info renogyInfo;
    const uint32_t num_data_registers = 35;
    const uint32_t num_info_registers = 17;

};

RenogyMPPT::RenogyMPPT(int modbus_address) {

    node.begin(modbus_address, Serial);
    // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
    node.setTransmitBuffer(0, lowWord(0));
    // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
    node.setTransmitBuffer(1, highWord(0));
}

void RenogyMPPT::renogy_read_data_registers() {

    uint8_t result;
    uint16_t data_registers[num_data_registers];

    result = node.readHoldingRegisters(0x103, num_data_registers);
    if (result == node.ku8MBSuccess)
    {

        this->renogyData.battery_soc = data_registers[0];
        this->renogyData.battery_voltage = data_registers[1] * .1; // will it crash if data_registers[1] doesn't exist?
        this->renogyData.battery_charging_amps = data_registers[2] * .1;

        this->renogyData.battery_charging_watts = this->renogyData.battery_voltage * this->renogyData.battery_charging_amps;

        //0x103 returns two bytes, one for battery and one for controller temp in c
        uint16_t raw_data = data_registers[3]; // eg 5913
        this->renogyData.controller_temperature = raw_data / 256;
        this->renogyData.battery_temperature = raw_data % 256;
        // for convenience, fahrenheit versions of the temperatures
        this->renogyData.controller_temperatureF = (this->renogyData.controller_temperature * 1.8) + 32;
        this->renogyData.battery_temperatureF = (this->renogyData.battery_temperature * 1.8) + 32;

        this->renogyData.load_voltage = data_registers[4] * .1;
        this->renogyData.load_amps = data_registers[5] * .01;
        this->renogyData.load_watts = data_registers[6];
        this->renogyData.solar_panel_voltage = data_registers[7] * .1;
        this->renogyData.solar_panel_amps = data_registers[8] * .01;
        this->renogyData.solar_panel_watts = data_registers[9];
        //Register 0x10A - Turn on load, write register, unsupported in wanderer - 10
        this->renogyData.min_battery_voltage_today = data_registers[11] * .1;
        this->renogyData.max_battery_voltage_today = data_registers[12] * .1;
        this->renogyData.max_charging_amps_today = data_registers[13] * .01;
        this->renogyData.max_discharging_amps_today = data_registers[14] * .1;
        this->renogyData.max_charge_watts_today = data_registers[15];
        this->renogyData.max_discharge_watts_today = data_registers[16];
        this->renogyData.charge_amphours_today = data_registers[17];
        this->renogyData.discharge_amphours_today = data_registers[18];
        this->renogyData.charge_watthours_today = data_registers[19];
        this->renogyData.discharge_watthours_today = data_registers[20];
        this->renogyData.controller_uptime_days = data_registers[21];
        this->renogyData.total_battery_overcharges = data_registers[22];
        this->renogyData.total_battery_fullcharges = data_registers[23];
        this->renogyData.last_update_time = millis();

        // Add these registers:
        // Registers 0x118 to 0x119- Total Charging Amp-Hours - 24/25    
        // Registers 0x11A to 0x11B- Total Discharging Amp-Hours - 26/27    
        // Registers 0x11C to 0x11D- Total Cumulative power generation (kWH) - 28/29    
        // Registers 0x11E to 0x11F- Total Cumulative power consumption (kWH) - 30/31    
        // Register  0x120 - Load Status, Load Brightness, Charging State - 32    
        // Registers 0x121 to 0x122 - Controller fault codes - 33/34
    }
    else
    {
        if (result == 0xE2) {
            Serial.println("Timed out reading the data registers!");
        }
        else {
            Serial.print("Failed to read the data registers... ");
            Serial.println(result, HEX); // E2 is timeout
        }
    }


}

void RenogyMPPT::renogy_read_info_registers() {
    int num_info_registers = this->num_info_registers;
    uint8_t result;
    uint16_t info_registers[num_info_registers];
    char buffer1[40], buffer2[40];
    uint8_t raw_data;

    result = node.readHoldingRegisters(0x00A, num_info_registers);
    if (result == node.ku8MBSuccess)
    {
        // read and process each value
        // Register 0x0A - Controller voltage and Current Rating - 0
        // Not sure if this is correct. I get the correct amp rating for my Wanderer 30 (30 amps), but I get a voltage rating of 0 (should be 12v)
        raw_data = info_registers[0];
        this->renogyInfo.voltage_rating = raw_data / 256;
        this->renogyInfo.amp_rating = raw_data % 256;
        this->renogyInfo.wattage_rating = this->renogyInfo.voltage_rating * this->renogyInfo.amp_rating;
        //Serial.println("raw ratings = " + String(raw_data));
        //Serial.println("Voltage rating: " + String(this->renogyInfo.voltage_rating));
        //Serial.println("amp rating: " + String(this->renogyInfo.amp_rating));

        //Register 0x0B - Controller discharge current and type - 1
        raw_data = info_registers[1];
        this->renogyInfo.discharge_amp_rating = raw_data / 256; // not sure if this should be /256 or /100
        this->renogyInfo.type = raw_data % 256; // not sure if this should be /256 or /100

        //Registers 0x0C to 0x13 - Product Model String - 2-9

        //Registers 0x014 to 0x015 - Software Version - 10-11
        itoa(info_registers[10], buffer1, 10);
        itoa(info_registers[11], buffer2, 10);
        strcat(buffer1, buffer2); // should put a divider between the two strings?
        strcpy(this->renogyInfo.software_version, buffer1);
        //Serial.println("Software version: " + String(this->renogyInfo.software_version));

        //Registers 0x016 to 0x017 - Hardware Version - 12-13
        itoa(info_registers[12], buffer1, 10);
        itoa(info_registers[13], buffer2, 10);
        strcat(buffer1, buffer2); // should put a divider between the two strings?
        strcpy(this->renogyInfo.hardware_version, buffer1);
        //Serial.println("Hardware version: " + String(this->renogyInfo.hardware_version));

        //Registers 0x018 to 0x019 - Product Serial Number - 14-15
        itoa(info_registers[14], buffer1, 10);
        itoa(info_registers[15], buffer2, 10);
        strcat(buffer1, buffer2); // should put a divider between the two strings?
        strcpy(this->renogyInfo.serial_number, buffer1);
        //Serial.println("Serial number: " + String(this->renogyInfo.serial_number)); // (I don't think this is correct)

        this->renogyInfo.modbus_address = info_registers[16];
        this->renogyInfo.last_update_time = millis();
    }
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
};

// TODO: LOAD CONTROLLER