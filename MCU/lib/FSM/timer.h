#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>


struct time {
    byte minutes;
    byte seconds;
};

void timerInit();
time getTime();
byte getSeconds();
byte getMinutes();

#endif