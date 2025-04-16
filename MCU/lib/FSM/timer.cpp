#include "timer.h"
#include "RTCZero.h"

RTCZero rtc;

byte getSeconds() {
    return rtc.getSeconds();
}

byte getMinutes() {
    return rtc.getMinutes();
}
