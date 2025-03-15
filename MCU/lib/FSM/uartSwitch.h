// 3.08.2025
// header for switching UART lines
#include <Arduino.h>

#ifndef SWITCH_H
#define SWITCH_H
#include "FSM.h"
#include "pins.h"
#include "victron.h"
#include "renogy.h"

#define SWITCH_DELAY 1500   // a general timer to allow the buffer to fill (if needed) and prevent blocking.

void uartSwitch(data& d, device dev, long baud, uint16_t config) {
    if (d.liftFlag || d.lowerFlag) {
        return; // don't interfere with running op.
    }

    if (d.currentDevice != dev) {   // if the line is active, do nothing.
        // disable the mux, select new lines.
        digitalWrite(MUX_DISABLE_0, HIGH);
        digitalWrite(MUX_DISABLE_1, HIGH);

        d.lastDevSwitchTime = millis();

        Serial1.flush();              // Wait for tx to clear.
        Serial1.begin(baud, config);  // change baud.
        while (Serial1.available()) {
            Serial1.read();  // clear input buffer.
        }

        digitalWrite(MUX_SEL_0, (dev & 0x001));
        digitalWrite(MUX_SEL_1, (dev & 0x010));
        digitalWrite(MUX_SEL_2, (dev & 0x100)); // ancillary in this case, we won't have this many lines.

        digitalWrite(MUX_DISABLE_0, LOW);
        digitalWrite(MUX_DISABLE_1, LOW);
    }
};

void winchControl(data& d) {
    unsigned long startTime = millis();
    unsigned long timeout = 10000;
    uint8_t index = 0;
    uint8_t numOfWinches = 4;
    for (uint8_t i = 0; i < numOfWinches; i++) {
        if (d.liftFlag[i]) {
            index = i;
            break;
        }
    }

    // check up/down from analogRead()
    // if high
        // index*2

    // select winch
    digitalWrite(MUX_DISABLE_2, HIGH);
    digitalWrite(MUX_SEL_0, (index & 0x001));
    digitalWrite(MUX_SEL_2, (index & 0x100)); // double check
    digitalWrite(MUX_SEL_1, (index & 0x010));
    digitalWrite(MUX_DISABLE_2, LOW);

    while (millis() - startTime < timeout) {
        // check sensor
        // if (analogRead height) { stop if too high or low }
            // activate winch fires relay;
    }


};

#ifdef DEBUG
int messageTest(data& data, bool useLED = false) {
    // attempts to read "TestMessage" at 19200 8N1, then 9600 8E1, then returns to 9600 8N1.
    // if use LED is true, the built in LED will be on when the second loop is entered. this indicates the sender can switch modes if not on a timer.

    String testMessage = "TestMessage";
    const size_t buffer_size = 64;    // on SAMD the rx buffer is 256 bytes total
    char buffer[buffer_size] = { '\0' };
    bool pass = false;
    int bytesToRead = 8;

    unsigned long start = millis();
    unsigned long timeout = 10000;

    // switch to BMS line
    uartSwitch(data, BMS, 19200, SERIAL_8N1);
    while (!pass) {
        // spins until testMessage is matched or timeout is reached.
        // with no delay in tx, this buffer may always contain junk and never complete.
        if (Serial1.available() > bytesToRead) {
            memset(buffer, '\0', buffer_size);
            Serial1.readBytes(buffer, buffer_size);
            for (size_t i = 0; i < buffer_size; i++) {
                if (buffer[i] == testMessage[0]) {
                    if (!String(buffer).substring(i, i + testMessage.length()).compareTo(testMessage)) {    // <-- ugly; sorry not sorry
                        pass = true;
                        break;
                    }
                }
            }
        }

        if (millis() - start > timeout) {
            uartSwitch(data, RADIO, 9600, SERIAL_8N1);
            return -1; // timeout 1
        }
    }

    if (useLED) {
        digitalWrite(LED_BUILTIN, HIGH);
    }

    pass = false;
    uartSwitch(data, MPPT, 9600, SERIAL_8E1);
    while (!pass) {
        // spins until testMessage is matched or timeout is reached.
        // with no delay in tx, this buffer may always contain junk and never complete.
        while (Serial1.available() > bytesToRead) {
            memset(buffer, '\0', buffer_size);
            Serial1.readBytes(buffer, buffer_size);
            for (size_t i = 0; i < buffer_size; i++) {
                if (buffer[i] == testMessage[0]) {
                    if (!String(buffer).substring(i, i + testMessage.length()).compareTo(testMessage)) {
                        pass = true;
                        break;
                    }
                }
            }
        }
        if (millis() - start > timeout) {
            uartSwitch(data, RADIO, 9600, SERIAL_8N1);
            if (useLED) {
                digitalWrite(LED_BUILTIN, LOW);
            }
            return -2;  // timeout 2
        }
    }

    uartSwitch(data, RADIO, 9600, SERIAL_8N1);    // must always happen
    if (useLED) {
        digitalWrite(LED_BUILTIN, LOW);
    }
    if (pass) {
        data.power_placeholder = 420;
        return 1;
    }
    else {
        data.power_placeholder = -1; //  ):
        return 0;
    }
};
#endif  // DEBUG
#endif  // SWITCH_H
