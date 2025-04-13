// 3.08.2025
// header for switching UART lines
#include <Arduino.h>

#ifndef SWITCH_H
#define SWITCH_H
#include "FSM.h"
#include "pins.h"

void uartSwitch(data& d, device dev, long baud, uint16_t config) {
    if (d.currentDevice != dev) {
        // if the line is active, do nothing.
        // disable the mux, select new lines.
        digitalWrite(MUX_DISABLE_0, HIGH);
        digitalWrite(MUX_DISABLE_1, HIGH);

        d.lastDevSwitchTime = millis();

        Serial1.flush();             // Wait for tx to clear.
        Serial1.begin(baud, config); // change baud.
        while (Serial1.available())
        {
            Serial1.read(); // clear input buffer.
        }

        digitalWrite(MUX_SEL_0, (dev & 0x001));
        digitalWrite(MUX_SEL_1, (dev & 0x010));
        digitalWrite(MUX_SEL_2, (dev & 0x100)); // ancillary in this case, we won't have this many lines.

        digitalWrite(MUX_DISABLE_0, LOW);   // rx
        digitalWrite(MUX_DISABLE_1, LOW);   // tx
    }
};
#endif // SWITCH_H
