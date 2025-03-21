
//Can add calculations for height after implimented
#include "pot_reader.h"

static uint8_t potPin;

void initPotentiometer(uint8_t pin) {
    potPin = pin;
    pinMode(potPin, INPUT);
}

int readPotentiometer() {
    return analogRead(potPin);
}
