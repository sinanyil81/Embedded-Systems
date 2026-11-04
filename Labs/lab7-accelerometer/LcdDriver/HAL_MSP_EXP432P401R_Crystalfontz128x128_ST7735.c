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
//*****************************************************************************
//
// HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.c -
//           Hardware abstraction layer for using the Educational Boosterpack's
//           Crystalfontz128x128 LCD with MSP-EXP432P401R LaunchPad
//
//*****************************************************************************

#include "HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <ti/grlib/grlib.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>

void HAL_LCD_PortInit(void)
{
    // LCD_SCK
    GPIO_setAsPeripheralModuleFunctionOutputPin(LCD_SCK_PORT, LCD_SCK_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    // LCD_MOSI
    GPIO_setAsPeripheralModuleFunctionOutputPin(LCD_MOSI_PORT, LCD_MOSI_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    // LCD_RST
    GPIO_setAsOutputPin(LCD_RST_PORT, LCD_RST_PIN);
    // LCD_RS
    GPIO_setAsOutputPin(LCD_DC_PORT, LCD_DC_PIN);
    // LCD_CS
    GPIO_setAsOutputPin(LCD_CS_PORT, LCD_CS_PIN);
}

void HAL_LCD_SpiInit(void)
{
    eUSCI_SPI_MasterConfig config =
        {
            EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
            LCD_SYSTEM_CLOCK_SPEED,
            LCD_SPI_CLOCK_SPEED,
            EUSCI_B_SPI_MSB_FIRST,
            EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
            EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
            EUSCI_B_SPI_3PIN
        };
    SPI_initMaster(LCD_EUSCI_BASE, &config);
    SPI_enableModule(LCD_EUSCI_BASE);

    GPIO_setOutputLowOnPin(LCD_CS_PORT, LCD_CS_PIN);

    GPIO_setOutputHighOnPin(LCD_DC_PORT, LCD_DC_PIN);
}


//*****************************************************************************
//
// Writes a command to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void HAL_LCD_writeCommand(uint8_t command)
{
    // Set to command mode
    GPIO_setOutputLowOnPin(LCD_DC_PORT, LCD_DC_PIN);

    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);

    // Transmit data
    UCB0TXBUF = command;

    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);

    // Set back to data mode
    GPIO_setOutputHighOnPin(LCD_DC_PORT, LCD_DC_PIN);
}


//*****************************************************************************
//
// Writes a data to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void HAL_LCD_writeData(uint8_t data)
{
    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);

    // Transmit data
    UCB0TXBUF = data;

    // USCI_B0 Busy? //
    while (UCB0STATW & UCBUSY);
}

//*****************************************************************************
//
//! Provides a small delay.
//!
//! \param ui32Count is the number of delay loop iterations to perform.
//!
//! This function provides a means of generating a delay by executing a simple
//! 3 instruction cycle loop a given number of times.  It is written in
//! assembly to keep the loop instruction count consistent across tool chains.
//!
//! It is important to note that this function does NOT provide an accurate
//! timing mechanism.  Although the delay loop is 3 instruction cycles long,
//! the execution time of the loop will vary dramatically depending upon the
//! application's interrupt environment (the loop will be interrupted unless
//! run with interrupts disabled and this is generally an unwise thing to do)
//! and also the current system clock rate and flash timings (wait states and
//! the operation of the prefetch buffer affect the timing).
//!
//! For best accuracy, a system timer should be used with code either polling
//! for a particular timer value being exceeded or processing the timer
//! interrupt to determine when a particular time period has elapsed.
//!
//! \return None.
//
//*****************************************************************************
#if defined( __ICCARM__ ) || defined(DOXYGEN)
void
SysCtlDelay(uint32_t ui32Count)
{
    __asm("    subs    r0, #1\n"
          "    bne.n   SysCtlDelay\n"
          "    bx      lr");
}
#endif
#if defined(codered) || defined( __GNUC__ ) || defined(sourcerygxx)
void __attribute__((naked))
SysCtlDelay(uint32_t ui32Count)
{
    __asm("    subs    r0, #1\n"
          "    bne     SysCtlDelay\n"
          "    bx      lr");
}
#endif
#if defined(rvmdk) || defined( __CC_ARM )
__asm void
SysCtlDelay(uint32_t ui32Count)
{
    subs    r0, #1;
    bne     SysCtlDelay;
    bx      lr;
}
#endif
