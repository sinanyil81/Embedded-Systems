#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>


/* ADC results buffer */
static uint16_t resultsBuffer[3];

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
     * with 3.3v reference */
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

    _accelSensorInit();
}

/*
 * Main function
 */
int main(void)
{
    _hwInit();


    while (1)
    {
        PCM_gotoLPM0();
    }
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
        /*Defining the conditions when device is moved UP and DOWN*/
        bool up_condition = resultsBuffer[2] > 13000 &&
                resultsBuffer[1] < 9000 && resultsBuffer[1] > 7000 &&
                resultsBuffer[0] < 9000 && resultsBuffer[0] > 7000;
        bool down_condition = resultsBuffer[2] < 10000 &&
                resultsBuffer[1] < 9000 && resultsBuffer[1] > 7000 &&
                resultsBuffer[0] < 9000 && resultsBuffer[0] > 7000;
        /*Checking the conditions and printing the recognition*/
        if(up_condition) {
            printf("UP\n");
            _delay_cycles(5000000);
        } else if(down_condition){
            printf("DOWN\n");
            _delay_cycles(5000000);
        }


    }
}
