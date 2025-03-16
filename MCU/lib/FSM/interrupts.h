#ifndef INTERRUPTS_H
#define INTERRUPTS_H

// These define's must be placed at the beginning before #include "SAMDTimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 1

// Select only one to be true for SAMD21. Must must be placed at the beginning before #include "SAMDTimerInterrupt.h"
#define USING_TIMER_TC3 false // Only TC3 can be used for SAMD51
#define USING_TIMER_TC4 false // Not to use with Servo library
#define USING_TIMER_TC5 true
#define USING_TIMER_TCC false
#define USING_TIMER_TCC1 false
#define USING_TIMER_TCC2 false // Don't use this, can crash on some boards

// #include <TimerInterrupt_Generic.h>
#include "SAMDTimerInterrupt.hpp"
//////////////////////////////////////////////

// TC3, TC4, TC5 max permissible TIMER_INTERVAL_MS is 1398.101 ms, larger will overflow, therefore not permitted
// Use TCC, TCC1, TCC2 for longer TIMER_INTERVAL_MS
// #define TIMER_INTERVAL_MS 1000 ---- using FSM macro for this instead

#if USING_TIMER_TC3
#define SELECTED_TIMER TIMER_TC3
#elif USING_TIMER_TC4
#define SELECTED_TIMER TIMER_TC4
#elif USING_TIMER_TC5
#define SELECTED_TIMER TIMER_TC5
#elif USING_TIMER_TCC
#define SELECTED_TIMER TIMER_TCC
#elif USING_TIMER_TCC1
#define SELECTED_TIMER TIMER_TCC1
#elif USING_TIMER_TCC2
#define SELECTED_TIMER TIMER_TCC
#else
#error You have to select 1 Timer
#endif

// Define the timer intervals for each interrupt in milliseconds
#define TIMER_INTERVAL_MS 1000
#define POWER_TIMER_INTERVAL_S 3
#define COMMS_MODULE_TIMER_INTERVAL_S 5 
#define EMERGENCY_LIFT_LOWER_TIMER_INTERVAL_MS 1000
#define CELLULAR_CHECKSERVER_TIMER_INTERVAL_MS 1000

//////////////////////////////////////////////

void PowerTimerHandler();
void CommsTimerHandler();
void EmergencyLiftLowerTimerHandler();

//////////////////////////////////////////////

void setupInterrupts();

// interrupt flag getters

bool getPowerFlag();
bool getCommsFlag();
bool getEmergencyLiftFlag();

// interrupt flag setters
void setPowerFlag(bool flag);
void setCommsFlag(bool flag);
void setEmergencyLiftFlag(bool flag);

#endif // INTERRUPTS_H