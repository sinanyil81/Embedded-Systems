/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
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
//****************************************************************************
//
// main.c - MSP-EXP432P401R + Educational Boosterpack MkII - Light Sensor
//
//          Displays lux value measured by the OPT3001 Digital Ambient
//          Light Sensor on the colored LCD. The MSP432 communicates
//          with the sensor through I2C.
//
//
//      *** Make sure J5 jumper on the BOOSTXL-EDUMKII is connected ***
//          to 3.LCD BACKLT
//
//****************************************************************************
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <stdio.h>

/* Graphic library context */
Graphics_Context g_sContext;

/* Timer_A Compare Configuration Parameter  (PWM) */
Timer_A_CompareModeConfig compareConfig_PWM = {
TIMER_A_CAPTURECOMPARE_REGISTER_3,          // Use CCR3
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        7500                                        // 50% Duty Cycle
        };

/* Timer_A Up Configuration Parameter */
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,                      // SMCLK = 3 MhZ
        TIMER_A_CLOCKSOURCE_DIVIDER_12,         // SMCLK/12 = 250 KhZ
        10000,                                  // 40 ms tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };

void _ledInit()
{
    /* Configures P2.4 to PM_TA0.1 for using Timer PWM to control LED */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,
    GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configures P2.6 to PM_TA0.3 for using Timer PWM to control LED */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6,
    GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configures P5.6 to PM_TA0.4 for using Timer PWM to control LED */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6,
    GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Timer_A0 for Up Mode and starting */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Configuring Timer_A2 for Up Mode and starting */
    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

    /* Initialize compare registers to generate PWM */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM); // For P2.6

    // For Port 2.4
    compareConfig_PWM.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    compareConfig_PWM.compareValue = 5000;
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM);

    // For Port 5.6
    compareConfig_PWM.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    compareConfig_PWM.compareValue = 2500;
    Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
}

void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

void _hwInit()
{
    /* Halting WDT and disabling master interrupts */
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Initializes Clock System */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    _graphicsInit();
    _ledInit();
}

/*
 * Main function
 */
int main(void)
{

    /* P2.6 and P2.4 and 5.6 are LEDs */
    _hwInit();

    Graphics_drawStringCentered(&g_sContext, (int8_t *) "LED Demo",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);

    while (1)
    {
    }
}
