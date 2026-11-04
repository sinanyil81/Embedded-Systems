#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include <stdio.h>

/* Graphic library context */
Graphics_Context g_sContext;

/* ADC results buffer */
static uint16_t resultsBuffer[3];

void drawTitle(void);
void drawAccelData(void);

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

void _accelSensorInit()
{
    /* Configures Pin 4.0, 4.2, and 6.1 as ADC input */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN0 | GPIO_PIN2,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    /* Initializing ADC (ADCOSC/64/8) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8,
                     0);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM2 (A11, A13, A14)  with no repeat)
     * with internal 2.5v reference */
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
    ADC14_configureConversionMemory(ADC_MEM0,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A14, ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(ADC_MEM1,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A13, ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(ADC_MEM2,
    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A11, ADC_NONDIFFERENTIAL_INPUTS);

    /* Enabling the interrupt when a conversion on channel 2 (end of sequence)
     *  is complete and enabling conversions */
    ADC14_enableInterrupt(ADC_INT2);

    /* Enabling Interrupts */
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    /* Setting up the sample timer to automatically step through the sequence
     * convert.
     */
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    /* Triggering the start of the sample */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
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
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    _graphicsInit();
    _accelSensorInit();
}

/*
 * Main function
 */
int main(void)
{
    _hwInit();

    drawTitle();

    while (1)
    {
        PCM_gotoLPM0();
    }
}

/*
 * Clear display and redraw title + accelerometer data
 */
void drawTitle()
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Accelerometer:",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);
    drawAccelData();
}

/*
 * Redraw accelerometer data
 */
void drawAccelData()
{
    char string[10];
    sprintf(string, "X: %5d", resultsBuffer[0]);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 8, 64, 50,
    OPAQUE_TEXT);

    sprintf(string, "Y: %5d", resultsBuffer[1]);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 8, 64, 70,
    OPAQUE_TEXT);

    sprintf(string, "Z: %5d", resultsBuffer[2]);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 8, 64, 90,
    OPAQUE_TEXT);

}

/* This interrupt is fired whenever a conversion is completed and placed in
 * ADC_MEM2. This signals the end of conversion and the results array is
 * grabbed and placed in resultsBuffer */
void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    /* ADC_MEM2 conversion completed */
    if (status & ADC_INT2)
    {
        /* Store ADC14 conversion results */
        resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
        resultsBuffer[2] = ADC14_getResult(ADC_MEM2);

        /*
         * Draw accelerometer data on display and determine if orientation
         * change thresholds are reached and redraw as necessary
         */
        if (resultsBuffer[0] < 5600)
        {
            if (Lcd_Orientation != LCD_ORIENTATION_LEFT)
            {
                Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_LEFT);
                drawTitle();
            }
            else
                drawAccelData();
        }
        else if (resultsBuffer[0] > 10400)
        {
            if (Lcd_Orientation != LCD_ORIENTATION_RIGHT)
            {
                Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_RIGHT);
                drawTitle();
            }
            else
                drawAccelData();
        }
        else if (resultsBuffer[1] < 5600)
        {
            if (Lcd_Orientation != LCD_ORIENTATION_UP)
            {
                Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
                drawTitle();
            }
            else
                drawAccelData();
        }
        else if (resultsBuffer[1] > 10400)
        {
            if (Lcd_Orientation != LCD_ORIENTATION_DOWN)
            {
                Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_DOWN);
                drawTitle();
            }
            else
                drawAccelData();
        }
        else
            drawAccelData();
    }
}
