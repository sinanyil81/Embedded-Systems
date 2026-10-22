#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

int main(void)
{
    volatile uint32_t i;

    // Stop watchdog timer
    WDT_A_holdTimer();

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* P1.1 as input for button */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    /* P1.4 as input for button */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);


    /* Enable interrupts on Port 1 (to catch P1.1 and P1.4 interrupts) */
    Interrupt_enableInterrupt(INT_PORT1);
    /* activate interrupt notification */
    Interrupt_enableMaster();

    while(1){
        /* low power mode */
        PCM_gotoLPM0();
    }
}

/* Port1 ISR */
void PORT1_IRQHandler(void)
{
    /* Check which pins generated the interrupts */
    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    /* clear interrupt flag (to clear pending interrupt indicator */
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    /* check if we received P1.1 or P1.4 interrupt */
    if((status & GPIO_PIN1) || (status & GPIO_PIN4)){
        /* Toggling the output on the LED */
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}
