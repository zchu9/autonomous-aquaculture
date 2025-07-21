#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>
#define POWER_SEND_INTERVAL 1 

struct time
{
    byte minutes;
    byte seconds;
    byte hours;
};

void timerInit();
time getTime();
byte getSeconds();
byte getMinutes();
byte getHours();

int getPowerFlag();
void setPowerFlag(int flag);

void powerInterrupt();
#endif