#include "msp.h"
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
 * MSP432 ADC14 - Single Channel Sample Repeat
 *
 * Description: This code example will demonstrate the basic functionality of
 * of the DriverLib ADC APIs with sampling a single channel repeatedly. Each
 * time the ADC conversion occurs, the result is stored into a local variable.
 * The sample timer is used to continuously grab a new value from the ADC
 * module using a manual iteration that is performed in the ADC ISR. A
 * normalized ADC value with respect to the 3.3v Avcc is also calculated using
 * the FPU.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P5.5  |<--- A0 (Analog Input)
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Statics */
static volatile uint16_t curADCResult;

int main(void)
{
    /* Halting the Watchdog  */
    WDT_A_holdTimer();

    /* Initializing Variables */
    curADCResult = 0;

    /* LED purposes */
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);

    /* Configuring GPIOs (5.5 A0) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5,
    GPIO_TERTIARY_MODULE_FUNCTION);

    /* Enable ADC block*/
    ADC14_enableModule();

    //![Single Sample Mode Configure]
    /* Initializing ADC (MCLK/1/4) */
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4,
            0);

    /* Configuring ADC Memory */
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADC_INPUT_A0, false);

    /* Configuring Sample Timer */
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
    //![Single Sample Mode Configure]

    /* Enabling interrupts */
    ADC14_enableInterrupt(ADC_INT0);
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    while (1)
    {
        PCM_gotoLPM0();
    }

}

 //![Single Sample Result]
/* ADC Interrupt Handler. This handler is called whenever there is a conversion
 * that is finished for ADC_MEM0.
 */
void ADC14_IRQHandler(void)
{
    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if (ADC_INT0 & status)
    {
        /* should be berween 0 and 16384*/
        curADCResult = ADC14_getResult(ADC_MEM0);

        if(curADCResult > 16000)
            GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
        else
            GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);

        /* next sample */
        ADC14_toggleConversionTrigger();
    }
}
//![Single Sample Result]

