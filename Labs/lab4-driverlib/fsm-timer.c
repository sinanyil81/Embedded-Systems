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
 * MSP432 PWM TA1.1-2, Up/Down Mode, DCO SMCLK
 *
 * Description: Toggle P1.0 using software and TA_0 ISR. Timer0_A is
 * configured for up mode, thus the timer overflows when TAR counts
 * to CCR0. In this example, CCR0 is loaded with 0x2DC6 which makes the LED
 * toggle every half a second..
 * ACLK = n/a, MCLK = SMCLK = default DCO ~1MHz
 * TACLK = SMCLK/64
 *
 *         MSP432P401
 *      -------------------
 *  /|\|                   |
 *   | |                   |
 *   --|RST                |
 *     |                   |
 *     |               P1.0|-->LED
 *     |                   |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void RedOn(){
    /* clear all the pins */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
    /* turn on RED */
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

void GreenOn(){
    /* clear all the pins */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* turn on GREEN */
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void BlueOn(){
    /* clear all the pins */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* turn on BLUE */
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

typedef enum {
    STATE_INIT,
    STATE_GREEN,
    STATE_RED,
    STATE_BLUE,
    NUM_STATES
}State_t;

typedef struct{
    State_t state;
    void (*state_function)(void);
} StateMachine_t;

void fn_INIT(void);
void fn_GREEN(void);
void fn_RED(void);
void fn_BLUE(void);

State_t current_state = STATE_INIT;

StateMachine_t fsm[] = {
                      {STATE_INIT, fn_INIT},
                      {STATE_GREEN, fn_GREEN},
                      {STATE_RED, fn_RED},
                      {STATE_BLUE, fn_BLUE}
};

void fn_INIT(){
       GreenOn();
        current_state = STATE_GREEN;
}

void fn_GREEN(){
        RedOn();
        current_state = STATE_RED;
}

void fn_RED(){
        BlueOn();
        current_state = STATE_BLUE;
}

void fn_BLUE(){
        GreenOn();
        current_state = STATE_GREEN;
}


/* Application Defines  */
#define TIMER_PERIOD    0x2DC6

/* Timer_A UpMode Configuration Parameter */
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_64,         // SMCLK/64 = 3MHz/64
        TIMER_PERIOD,                           // every half second
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

int main(void)
{
    /* Stop WDT  */
    WDT_A_holdTimer();

    /* Configuring P2.0, 2.1, 2.2 as output */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    /* set to LOW */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* Configuring Timer_A1 for Up Mode */
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    /* Enabling interrupts and starting the timer */
    // Interrupt_enableSleepOnIsrExit();
    Interrupt_enableInterrupt(INT_TA1_0);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();

    /* Sleeping when not in use */
    while (1)
    {
        PCM_gotoLPM0();
        if(current_state < NUM_STATES){
            (*fsm[current_state].state_function)();
        }
        else{
            /* serious error */
        }
    }
}

void TA1_0_IRQHandler(void)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
            TIMER_A_CAPTURECOMPARE_REGISTER_0);
}
