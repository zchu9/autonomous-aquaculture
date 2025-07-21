#include "FSM.h"
#include <Arduino.h>
data d;

void setup() {
    initializeStartup(d);
}

void loop()
{
    FSM(d);
}
