#ifndef TIMERS_H
#define TIMERS_H

#include <Arduino.h>

volatile static bool one_sec = false;

byte getSeconds();
byte getMinutes();

// a simple oneshot timer to dump SAMD_TimerInterupt lib and enable sleep from LowPower and wakeup on RTC events
// SAMD21 timer/counter starts on p555 of the manual. p559 contains startup details. http://www.technoblogy.com/show?3RC9
// initializes hardware timers; shouldn't be necessary from here on out. TODO: remove in final builds.
void setupTimer() {
    // Configure asynchronous clock source
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TCC2_TC3_Val;    // select TC3 peripheral channel
    GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK0;        // select source GCLK_GEN[0]
    GCLK->CLKCTRL.bit.CLKEN = 1;            // enable TC3 generic clock

    // Configure synchronous bus clock
    PM->APBCSEL.bit.APBCDIV = 0;            // no prescaler
    PM->APBCMASK.bit.TC3_ = 1;                // enable TC3 interface

    // Configure Count Mode (16-bit)
    TC3->COUNT16.CTRLA.bit.MODE = 0x0;

    // Configure Prescaler for divide by 2 (500kHz clock to COUNT)
    TC3->COUNT16.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV1024_Val;

    // Configure TC3 Compare Mode for compare channel 0
    TC3->COUNT16.CTRLA.bit.WAVEGEN = 0x1;            // "Match Frequency" operation

    // Initialize compare value for 100mS @ 500kHz
    TC3->COUNT16.CC[0].reg = 46874;

    // Enable TC3 compare mode interrupt generation
    TC3->COUNT16.INTENSET.bit.MC0 = 0x1;    // Enable match interrupts on compare channel 0 

    // Enable TC3
    TC3->COUNT16.CTRLA.bit.ENABLE = 1;

    // Wait until TC3 is enabled
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY == 1);

    /* Set TC3 Interrupt Priority to Level 3 */
    NVIC_SetPriority(TC3_IRQn, 0);

    /* Enable TC3 NVIC Interrupt Line */
    NVIC_EnableIRQ(TC3_IRQn);
}

void TC3_Handler(void) {
    static int count = 0;
    count++;
    if (count % 2 == 0) {
        one_sec = true;
    }

    // Acknowledge the interrupt (clear MC0 interrupt flag to re-arm)
    TC3->COUNT16.INTFLAG.reg |= 0b00010000;
}
#endif