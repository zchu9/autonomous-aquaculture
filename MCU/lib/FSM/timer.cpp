#include "timer.h"
#include "RTCZero.h"

RTCZero rtc;
static int seconds = 0;
static int minutes = 0;
static int checkPowerFlag = 0;

void timerInit()
{

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
    Serial.println("Time: " + String(getHours()) + ":" + String(getMinutes()) + ":" + String(getSeconds()));
    if (minutes % 10 == 0)
    {
        checkPowerFlag = 1;
        Serial.println("Check Power Flag set to 1");
    }
}