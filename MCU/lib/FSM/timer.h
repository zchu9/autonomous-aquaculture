// a simple oneshot timer to dump SAMD_TimerInterupt lib and enable sleep from LowPower and wakeup on RTC events
// SAMD21 timer/counter starts on p555 of the manual. p559 contains startup details. http://www.technoblogy.com/show?3RC9

#include <Arduino.h>
volatile static bool one_sec = false;

void setupTimer() {
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3;  // Use GCLK0 for TC3

    TC3->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;    // Reset TC3
    while (TC3->COUNT16.CTRLA.bit.SWRST);       // Wait for completion

    TC3->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ | TC_CTRLA_PRESCALER_DIV1024;    // 16-bit mode, MFRQ mode, / 1024

    TC3->COUNT16.CC[0].reg = 46874;                         // Divide clock by 46875
    while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);   // Synchronize
    
    
    // set oneshot mode such that the timer stops on overflow.
    TC3->COUNT16.CTRLBSET.bit.ONESHOT = 1;
    while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);   // Synchronize
    
    // enable interrupts    
    NVIC_DisableIRQ(TC3_IRQn);
    NVIC_ClearPendingIRQ(TC3_IRQn);
    NVIC_SetPriority(TC3_IRQn, 0);
    NVIC_EnableIRQ(TC3_IRQn);
    
    // Enable the TC3 interrupt request
    TC3->COUNT16.INTENSET.bit.MC0 = 1;
    while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);   // Synchronize

    TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;              // Enable TC3
    while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);   // Synchronize
}

void startTimer() {
    TC3->COUNT16.CTRLBSET.bit.CMD = TC_CTRLBCLR_CMD_RETRIGGER;
    while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);   // Synchronize
}

void TC3_Handler() {
    if (TC3->COUNT16.INTFLAG.bit.MC0 == 1) {
        one_sec = true;
    }
    TC3->COUNT16.INTFLAG.bit.MC0 = 1;   // allow the timer to run again;
}