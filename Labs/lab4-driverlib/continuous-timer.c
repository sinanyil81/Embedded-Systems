/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 Timer_A - Continuous Overflow Interrupt
 *
 *  Description: Toggle P1.0 using software and the Timer0_A overflow ISR.
 *  In this example an ISR triggers when TA overflows. Inside the ISR P1.0
 *  is toggled. Toggle rate is exactly 0.5Hz.
 *
 *  ACLK = TACLK = 32768Hz, MCLK = SMCLK = DCO = 3MHz
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> P1.0 LED
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 ******************************************************************************/
/* DriverLib Includes */
#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Statics */
const Timer_A_ContinuousModeConfig continuousModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,           // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,      // ACLK/1 = 32.768khz
        TIMER_A_TAIE_INTERRUPT_ENABLE,      // Enable Overflow ISR
        TIMER_A_DO_CLEAR                    // Clear Counter
};

int main(void)
{
    /* Stop watchdog timer */
    WDT_A_holdTimer();

    /* Configuring P1.0 as output */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Starting and enabling ACLK (32kHz) */
    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Configuring Continuous Mode */
    Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);

    /* Enabling interrupts and going to sleep */
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(INT_TA0_N);

    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();

    /* Starting the Timer_A0 in continuous mode */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);

    while(1){
        PCM_gotoLPM0();
    }

}

//******************************************************************************
//
//This is the TIMERA interrupt vector service routine.
//
//******************************************************************************
void TA0_N_IRQHandler(void)
{
    /* clear the timer pending interrupt flag */
    Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    /* toggle LED */
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}
