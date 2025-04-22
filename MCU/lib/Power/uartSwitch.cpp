#include "uartSwitch.h"

void initMuxPins()
{
    pinMode(MUX_DISABLE_0, OUTPUT);
    pinMode(MUX_DISABLE_1, OUTPUT);
    pinMode(MUX_SEL_0, OUTPUT);
    pinMode(MUX_SEL_1, OUTPUT);
    pinMode(MUX_SEL_2, OUTPUT);

    digitalWrite(MUX_DISABLE_0, LOW); // enable the mux
    digitalWrite(MUX_DISABLE_1, LOW);
}

void uartSwitch(device dev, long baud, uint16_t config)
{
    // disable the mux, select new lines.
    digitalWrite(MUX_DISABLE_0, HIGH);
    digitalWrite(MUX_DISABLE_1, HIGH);

    // Serial1.flush();             // Wait for tx to clear.
    Serial1.begin(baud, config); // change baud.
    while (Serial1.available())
    {
        Serial1.read(); // clear input buffer.
    }

    digitalWrite(MUX_SEL_0, (dev & 0x001));
    digitalWrite(MUX_SEL_1, (dev & 0x010));
    digitalWrite(MUX_SEL_2, (dev & 0x100)); // ancillary in this case, we won't have this many lines.
    digitalWrite(MUX_DISABLE_0, LOW);       // rx
    digitalWrite(MUX_DISABLE_1, LOW);       // tx
}