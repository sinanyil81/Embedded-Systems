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
        TIMER_A_CAPTURECOMPARE_REGISTER_4,          // Use CCR3
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        10000                                        // 25% Duty Cycle initially
        };

/* Timer_A Up Configuration Parameter */
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,                      // SMCLK = 3 MhZ
        TIMER_A_CLOCKSOURCE_DIVIDER_12,         // SMCLK/12 = 250 KhZ
        20000,                                  // 40 ms tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };

void _buzzerInit()
{
    /* Configures P2.7 to PM_TA0.4 for using Timer PWM to control the buzzer */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7,
    GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Timer_A0 for Up Mode and starting */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    /* Initialize compare registers to generate PWM */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig_PWM); // For P2.7
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
    _buzzerInit();

}

/*
 * Main function
 */
int main(void)
{

    /* P2.7 buzzer */
    _hwInit();

    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Buzzer Demo",
    AUTO_STRING_LENGTH,
                                64, 30,
                                OPAQUE_TEXT);

    while (1)
    {
    }
}
