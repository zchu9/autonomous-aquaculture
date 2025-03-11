#include <Arduino.h>
#include <renogyFunctions.h>

void getDeviceAddress() {
    // modbus devices have an address between 1 and 247; 0 is global broadcast.
    // polling all devices is necessary to find our device.
    uint8_t startAddress = 0x01;
    uint8_t endAddress = 0xF8;

    // recieve buffer
    // const size_t buffer_size = 128;
    // uint8_t buffer[buffer_size] = { '\0' }; 

    // modbus message; attempts to read system voltage/current at 0x000A.
    const size_t message_size = 8;
    uint8_t message[message_size] = { '\0', READREG, 0, 0xA, 0 , 1, '\0', '\0' };
    char serialMessage[message_size];

    // TODO: configure UART
    // then send to each address and check for a response.
    for (uint8_t i = startAddress; i < endAddress; i++) {
        message[0] = i; // address
        int crc = genCRC(message, 6);
        message[6] = crc & 0xFF;    // crc low
        message[7] = (crc >> 8) & 0xFF; // crc high
        memcpy(serialMessage, message, sizeof(uint8_t));    // u_char cannot be cast to char; this avoids cast errors.
        while(Serial1.available() > 0) {
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


void readRenogyPowerData() {
    // validate correct line and protocol
    // send address + function
    // wait, recieve bytes
    // crc validate
    // repeat for each stat as needed.
};
