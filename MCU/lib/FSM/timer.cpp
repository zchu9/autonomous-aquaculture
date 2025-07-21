#include "timer.h"
#include "RTCZero.h"
#include <ArduinoTrace.h>

RTCZero rtc;
static volatile int seconds = 0;
static volatile int minutes = 0;
static volatile int checkPowerFlag = 0;

void timerInit()
{
    TRACE();
    rtc.begin();
    minutes = getMinutes() + 1;
    rtc.setAlarmSeconds(seconds);
    rtc.setAlarmMinutes(minutes);
    rtc.enableAlarm(rtc.MATCH_MMSS);
    rtc.attachInterrupt(powerInterrupt);
}

time getTime()
{
    time t;
    t.seconds = getSeconds();
    t.minutes = getMinutes();
    t.hours = getHours();
    return t;
}

byte getSeconds()
{
    return rtc.getSeconds();
}

byte getMinutes()
{
    return rtc.getMinutes();
}

byte getHours()
{
    return rtc.getHours();
}

int getPowerFlag()
{
    return checkPowerFlag;
}

void setPowerFlag(int flag)
{
    checkPowerFlag = flag;
}

void powerInterrupt()
{
    rtc.setAlarmMinutes(minutes = minutes + 1 % 60);
// Serial.println("Time: " + String(getHours()) + ":" + String(getMinutes()) + ":" + String(getSeconds()));
    if (minutes % POWER_SEND_INTERVAL == 0)
    {
        checkPowerFlag = 1;
// Serial.println("Check Power Flag set to 1");
    }
}