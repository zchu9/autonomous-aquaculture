// 3.08.2025
// header for switching UART lines

#ifndef SWITCH_H
#define SWITCH_H
#include <Arduino.h>
#include "pins.h"

enum device
{
    RADIO,
    MPPT,
    BMS,
}; // LoRA module, renogy mppt, victron bms.

void initMuxPins();

void uartSwitch(device dev, long baud, uint16_t config);

#endif // SWITCH_H
