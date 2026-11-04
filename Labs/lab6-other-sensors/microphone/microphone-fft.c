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
// main.c - MSP-EXP432P401R + Educational Boosterpack MkII - Microphone FFT
//
//          CMSIS DSP Software Library is used to perform 512-point FFT on
//          the audio samples collected with MSP432's ADC14 from the Education
//          Boosterpack's onboard microhpone. The resulting frequency bin data
//          is displayed on the BoosterPack's 128x128 LCD.
//
//****************************************************************************

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <stdio.h>

#define ARM_MATH_CM4

#include <arm_math.h>
#include <arm_const_structs.h>

#define TEST_LENGTH_SAMPLES 512
#define SAMPLE_LENGTH 512

/* ------------------------------------------------------------------
 * Global variables for FFT Bin Example
 * ------------------------------------------------------------------- */
uint32_t fftSize = SAMPLE_LENGTH;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
volatile arm_status status;

/* Graphic library context */
Graphics_Context g_sContext;

#define SMCLK_FREQUENCY     48000000
#define SAMPLE_FREQUENCY    8000

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[32];

/* FFT data/processing buffers*/
float hann[SAMPLE_LENGTH];
int16_t data_array1[SAMPLE_LENGTH];
int16_t data_array2[SAMPLE_LENGTH];
int16_t data_input[SAMPLE_LENGTH * 2];
int16_t data_output[SAMPLE_LENGTH];

volatile int switch_data = 0;

uint32_t color = 0;

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig =
{
    TIMER_A_CLOCKSOURCE_SMCLK,
    TIMER_A_CLOCKSOURCE_DIVIDER_1,
    (SMCLK_FREQUENCY / SAMPLE_FREQUENCY),
    TIMER_A_CAPTURECOMPARE_REGISTER_1,
    TIMER_A_OUTPUTMODE_SET_RESET,
    (SMCLK_FREQUENCY / SAMPLE_FREQUENCY) / 2
};

int main(void)
{
    /* Halting WDT and disabling master interrupts */
    MAP_WDT_A_holdTimer();
    MAP_Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1*/
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Initializes Clock System */
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);

    /* Draw Title, x-axis, gradation & labels */
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawLineH(&g_sContext, 0, 127, 115);
    Graphics_drawLineV(&g_sContext, 0, 115, 117);
    Graphics_drawLineV(&g_sContext, 16, 115, 116);
    Graphics_drawLineV(&g_sContext, 31, 115, 117);
    Graphics_drawLineV(&g_sContext, 32, 115, 117);
    Graphics_drawLineV(&g_sContext, 48, 115, 116);
    Graphics_drawLineV(&g_sContext, 63, 115, 117);
    Graphics_drawLineV(&g_sContext, 64, 115, 117);
    Graphics_drawLineV(&g_sContext, 80, 115, 116);
    Graphics_drawLineV(&g_sContext, 95, 115, 117);
    Graphics_drawLineV(&g_sContext, 96, 115, 117);
    Graphics_drawLineV(&g_sContext, 112, 115, 116);
    Graphics_drawLineV(&g_sContext, 127, 115, 117);

    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"512-Point FFT",
                                AUTO_STRING_LENGTH,
                                64,
                                6,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"0",
                                AUTO_STRING_LENGTH,
                                4,
                                122,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"1",
                                AUTO_STRING_LENGTH,
                                32,
                                122,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"2",
                                AUTO_STRING_LENGTH,
                                64,
                                122,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"3",
                                AUTO_STRING_LENGTH,
                                96,
                                122,
                                OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"4",
                                AUTO_STRING_LENGTH,
                                125,
                                122,
                                OPAQUE_TEXT);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_drawStringCentered(&g_sContext,
                                (int8_t *)"kHz",
                                AUTO_STRING_LENGTH,
                                112,
                                122,
                                OPAQUE_TEXT);

    // Initialize Hann Window
    int n;
    for(n = 0; n < SAMPLE_LENGTH; n++)
    {
        hann[n] = 0.5f - 0.5f * cosf((2 * PI * n) / (SAMPLE_LENGTH - 1));
    }

    /* Configuring Timer_A to have a period of approximately 500ms and
     * an initial duty cycle of 10% of that (3200 ticks)  */
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    /* Initializing ADC (MCLK/1/1) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);

    /* Configuring GPIOs (4.3 A10) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN3,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A10, false);

    /* Set ADC result format to signed binary */
    ADC14_setResultFormat(ADC_SIGNED_BINARY);

    /* Configuring DMA module */
    DMA_enableModule();
    DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);

    DMA_disableChannelAttribute(DMA_CH7_ADC14,
                                UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                UDMA_ATTR_HIGH_PRIORITY |
                                UDMA_ATTR_REQMASK);

    /* Setting Control Indexes. In this case we will set the source of the
     * DMA transfer to ADC14 Memory 0
     *  and the destination to the
     * destination data array. */
    DMA_setChannelControl(
        UDMA_PRI_SELECT | DMA_CH7_ADC14,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_1);
    DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH7_ADC14,
                               UDMA_MODE_PINGPONG, (void*) &ADC14->MEM[0],
                               data_array1, SAMPLE_LENGTH);

    DMA_setChannelControl(
        UDMA_ALT_SELECT | DMA_CH7_ADC14,
        UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
        UDMA_DST_INC_16 | UDMA_ARB_1);
    DMA_setChannelTransfer(UDMA_ALT_SELECT | DMA_CH7_ADC14,
                               UDMA_MODE_PINGPONG, (void*) &ADC14->MEM[0],
                               data_array2, SAMPLE_LENGTH);

    /* Assigning/Enabling Interrupts */
    DMA_assignInterrupt(DMA_INT1, 7);
    Interrupt_enableInterrupt(INT_DMA_INT1);
    DMA_assignChannel(DMA_CH7_ADC14);
    DMA_clearInterruptFlag(7);
    Interrupt_enableMaster();

    /* Now that the DMA is primed and setup, enabling the channels. The ADC14
     * hardware should take over and transfer/receive all bytes */
    DMA_enableChannel(7);
    ADC14_enableConversion();

    while(1)
    {
        PCM_gotoLPM0();

        int i = 0;

        /* Computer real FFT using the completed data buffer */
        if(switch_data & 1)
        {
            for(i = 0; i < 512; i++)
            {
                data_array1[i] = (int16_t)(hann[i] * data_array1[i]);
            }
            arm_rfft_instance_q15 instance;
            status = arm_rfft_init_q15(&instance, fftSize, ifftFlag,
                                       doBitReverse);

            arm_rfft_q15(&instance, data_array1, data_input);
        }
        else
        {
            for(i = 0; i < 512; i++)
            {
                data_array2[i] = (int16_t)(hann[i] * data_array2[i]);
            }
            arm_rfft_instance_q15 instance;
            status = arm_rfft_init_q15(&instance, fftSize, ifftFlag,
                                       doBitReverse);

            arm_rfft_q15(&instance, data_array2, data_input);
        }

        /* Calculate magnitude of FFT complex output */
        for(i = 0; i < 1024; i += 2)
        {
            data_output[i /
                        2] =
                (int32_t)(sqrtf((data_input[i] *
                                 data_input[i]) +
                                (data_input[i + 1] * data_input[i + 1])));
        }

        q15_t maxValue;
        uint32_t maxIndex = 0;

        arm_max_q15(data_output, fftSize, &maxValue, &maxIndex);

        if(maxIndex <= 64)
        {
            color = ((uint32_t)(0xFF * (maxIndex / 64.0f)) << 8) + 0xFF;
        }
        else if(maxIndex <= 128)
        {
            color =
                (0xFF - (uint32_t)(0xFF * ((maxIndex - 64) / 64.0f))) + 0xFF00;
        }
        else if(maxIndex <= 192)
        {
            color =
                ((uint32_t)(0xFF * ((maxIndex - 128) / 64.0f)) << 16) + 0xFF00;
        }
        else
        {
            color =
                ((0xFF -
                  (uint32_t)(0xFF *
                             ((maxIndex - 192) / 64.0f))) << 8) + 0xFF0000;
        }

        /* Draw frequency bin graph */
        for(i = 0; i < 256; i += 2)
        {
            int x = min(100, (int)((data_output[i] + data_output[i + 1]) / 8));

            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
            Graphics_drawLineV(&g_sContext, i / 2, 114 - x, 14);
            Graphics_setForegroundColor(&g_sContext, color);
            Graphics_drawLineV(&g_sContext, i / 2, 114, 114 - x);
        }
    }
}

/* Completion interrupt for ADC14 MEM0 */
void DMA_INT1_IRQHandler(void)
{
    /* Switch between primary and alternate bufferes with DMA's PingPong mode */
    if(DMA_getChannelAttribute(7) & UDMA_ATTR_ALTSELECT)
    {
        DMA_setChannelControl(
            UDMA_PRI_SELECT | DMA_CH7_ADC14,
            UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
            UDMA_DST_INC_16 | UDMA_ARB_1);
        DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH7_ADC14,
                               UDMA_MODE_PINGPONG, (void*) &ADC14->MEM[0],
                               data_array1, SAMPLE_LENGTH);
        switch_data = 1;
    }
    else
    {
        DMA_setChannelControl(
            UDMA_ALT_SELECT | DMA_CH7_ADC14,
            UDMA_SIZE_16 | UDMA_SRC_INC_NONE |
            UDMA_DST_INC_16 | UDMA_ARB_1);
        DMA_setChannelTransfer(UDMA_ALT_SELECT | DMA_CH7_ADC14,
                               UDMA_MODE_PINGPONG, (void*) &ADC14->MEM[0],
                               data_array2, SAMPLE_LENGTH);
        switch_data = 0;
    }
}
