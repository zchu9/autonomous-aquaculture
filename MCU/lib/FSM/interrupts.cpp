// interrupts.cpp
// Implementation of interrupt service routines

#include "interrupts.h"
volatile bool powerFlag = false;
volatile bool commsFlag = false;
volatile bool emergencyLiftFlag = false;

#define DEBUG_LOCAL 0

void PowerTimerHandler()
{
#if DEBUG_LOCAL
    Serial.print(F("Power Timer Interrupt, millis() = "));
    Serial.println(millis());
#endif
    static int psec = 0;
    static int csec = 0;
    if (psec == POWER_TIMER_INTERVAL_S)
    {
        psec = 0;
        powerFlag = true;
    }
    else
    {
        psec++;
    }

    if (csec == COMMS_MODULE_TIMER_INTERVAL_S)
    {
        csec = 0;
        commsFlag = true;
    }
    else
    {
        csec++;
    }
}

void CommsTimerHandler()
{
    commsFlag = true;
}

void EmergencyLiftLowerTimerHandler()
{
    emergencyLiftFlag = true;
}

// Init selected SAMD timer
SAMDTimer powerTimer(SELECTED_TIMER);
SAMDTimer commsTimer(SELECTED_TIMER);
uint32_t preMillisTimer = 0;
void setupInterrupts()
{
    // Initialize the timer with the selected timer and interval
    // while (millis() < 5000)
    //     ;
    if (powerTimer.attachInterruptInterval_MS(TIMER_INTERVAL_MS, PowerTimerHandler)) // TODO: change interval
    {
        preMillisTimer = millis();
        Serial.print(F("Starting PowerTimer OK, millis() = "));
        Serial.println(preMillisTimer);
    }
    else
        Serial.println(F("Can't set PowerTimer. Select another freq. or timer"));
}

bool getPowerFlag()
{
    return powerFlag;
}

bool getCommsFlag()
{
    return commsFlag;
}

bool getEmergencyLiftFlag()
{
    return emergencyLiftFlag;
}

void setPowerFlag(bool flag)
{
#if DEBUG_LOCAL
    Serial.print(F("Power Flag set to "));
    Serial.println(flag);
#endif
    powerFlag = flag;
}
void setCommsFlag(bool flag)
{
    commsFlag = flag;
}
void setEmergencyLiftFlag(bool flag)
{
    emergencyLiftFlag = flag;
}
