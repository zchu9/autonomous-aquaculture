#include "timer.h"
#include "RTCZero.h"

RTCZero rtc;

void timerInit() {
    rtc.begin();
}

time getTime(){
    time t;
    t.seconds = getSeconds();
    t.minutes = getMinutes();
    return t;
}

byte getSeconds() {
    return rtc.getSeconds();
}

byte getMinutes() {
    return rtc.getMinutes();
}
