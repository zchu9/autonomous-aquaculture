// 3.08.2025
// header for switching UART lines
#include <Arduino.h>

#ifndef SWITCH_H
#define SWITCH_H
#include "FSM.h"

// set pinMode to OUTPUT in main.
// currently unassigned, move to a dedicated pins header when merged with sensors;
#define MUX_SEL_0 -1
#define MUX_SEL_1 -1
#define MUX_SEL_2 -1

#define MUX_DISABLE_0 -1    // RX
#define MUX_DISABLE_1 -1    // TX
#define MUX_DISABLE_2 -1    // Winch

void uartSwitch(device d, long baud, uint16_t config) {
    // disable the mux, select new lines.
#ifndef DEBUG
    digitalWrite(MUX_DISABLE_0, HIGH);
    digitalWrite(MUX_DISABLE_1, HIGH);
    digitalWrite(MUX_SEL_0, (d & 0x001));
    digitalWrite(MUX_SEL_1, (d & 0x010));
    digitalWrite(MUX_SEL_2, (d & 0x100));
#endif

    Serial1.flush();              // Wait for tx to clear.
    Serial1.begin(baud, config);  // change baud.
    while (Serial1.available()) {
        Serial1.read();  // clear input buffer.
    }

#ifndef DEBUG
    digitalWrite(MUX_DISABLE_0, LOW);
    digitalWrite(MUX_DISABLE_1, LOW);
#endif
};

#ifdef DEBUG
int messageTest(data& data) {
    size_t buffer_size = 30;
    char buffer[buffer_size];
    char testMessage[] = "TestMessage";
    int chars = 0, messageLen = (int)(sizeof(testMessage) / sizeof(testMessage[0]));
    bool passA = false, passB = false;

    // naive timeout
    int timeout = 10000;
    int count = 0;

    // switch to BMS line
    uartSwitch(BMS, 19200, SERIAL_8N1);
    while (!passA) {
        while (Serial1.available()) {
            chars = Serial1.readBytes(buffer, buffer_size);
            if (chars != 0) {
                for (int i = 0; i < messageLen; i++) {
                    if (testMessage[i] != buffer[i]) {
                        passA = false;
                        break;
                    }
                    passA = true;
                }
                snprintf(buffer, buffer_size, "t:%lu | passed 1", millis());
                Serial.println(buffer);
            }
        }

        delay(20);  // arbitrary;
        count++;
        if (count == timeout) {
            Serial.println("timeout1");
            count = 0;
            break;
        }
    }

    // switch to the mppt line
    uartSwitch(MPPT, 9600, SERIAL_8E1);
    count = 0;
    while (!passB) {
        while (Serial1.available()) {
            chars = Serial1.readBytes(buffer, buffer_size);
            if (chars != 0) {
                for (int i = 0; i < messageLen; i++) {
                    if (testMessage[i] != buffer[i]) {
                        passB = false;
                        break;
                    }
                    passB = true;
                }
                snprintf(buffer, buffer_size, "t:%lu | passed 2", millis());
                Serial.println(buffer);
            }
        }

        delay(20);  // arbitrary;
        count++;
        if (count == timeout) {
            Serial.println("timeout2");
            count = 0;
            break;
        }
    }

    if (passA && passB) {
        data.power_placeholder = 420;
        return 1;
    }
    else {
        data.power_placeholder = -1; //  ):
        return 0;
    }

    // TODO: add a radio segment to change the line to the LoRA module.
    // uartSwitch(RADIO, 9600, SERIAL_8N1);
};
#endif  // DEBUG
#endif  // SWITCH_H
