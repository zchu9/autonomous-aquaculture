// interrupts.cpp
// Implementation of interrupt service routines

#include "interrupts.h"
volatile bool powerFlag = false;
volatile bool commsFlag = false;
volatile bool emergencyLiftFlag = false;

#define DEBUG_LOCAL 1

void PowerTimerHandler() {
#if DEBUG_LOCAL
    Serial.print(F("Power Timer Interrupt, millis() = "));
    Serial.println(millis());
#endif
    static int sec = 0;
    if (sec == POWER_TIMER_INTERVAL_MS / 1000) {
        sec = 0;
        powerFlag = true;
    }
    else {
        sec++;
    }
}

void CommsTimerHandler() {
    commsFlag = true;
}

void EmergencyLiftLowerTimerHandler() {
    emergencyLiftFlag = true;
}

// Init selected SAMD timer
SAMDTimer powerTimer(SELECTED_TIMER);
uint32_t preMillisTimer = 0;
void setupInterrupts() {
    // Initialize the timer with the selected timer and interval
    while (millis() < 5000)
        ;
    if (powerTimer.attachInterruptInterval_MS(1000, PowerTimerHandler)) // TODO: change interval
    {
        preMillisTimer = millis();
        Serial.print(F("Starting ITimer OK, millis() = "));
        Serial.println(preMillisTimer);
    }
    else
        Serial.println(F("Can't set ITimer. Select another freq. or timer"));
}

bool getPowerFlag() {
    return powerFlag;
}

bool getCommsFlag() {
    return commsFlag;
}

bool getEmergencyLiftFlag() {
    return emergencyLiftFlag;
}

void setPowerFlag(bool flag) {
#if DEBUG_LOCAL
    Serial.print(F("Power Flag set to "));
    Serial.println(flag);
#endif
    powerFlag = flag;
}
void setCommsFlag(bool flag) {
    commsFlag = flag;
}
void setEmergencyLiftFlag(bool flag) {
    emergencyLiftFlag = flag;
}
