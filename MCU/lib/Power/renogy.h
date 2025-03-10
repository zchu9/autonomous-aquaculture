// 3.01.2025
// placeholder header for the Rover MPPT data
// Renogy Modbus RTU Protocol

// format:
// start . address . function . data    . error check . end
// 10ms  . 1 byte  . 1 byte   . 2 bytes . 2 byte   . 10ms

// #include <Arduino.h>
// #include "CRC.h"

#define renogyBaud 9600
#define renogyConfig SERIAL_8E1 // modbus *requires* even parity or 2 stop bits.

// to be expanded with additional function codes from ROVER_MODBUS.PDF
typedef enum {
    panelV = 0x0107,
    panelA = 0x0108,
    panelW = 0x0109,
} funcCodes;

struct renogyDevice
{
    unit16_t deviceAddress;
};

void getDeviceAddress();
void readRenogyPowerData() {
    // validate correct line and protocol
    // send address + function
    // wait, recieve bytes, crc.add(b)
    // crc validate
    // repeat for each stat as needed.
};
