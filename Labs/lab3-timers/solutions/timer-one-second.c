#include "msp.h"

void configurePorts(){
    /* configure P2.0, P2.1 and P2.2 as GPIO */
    P2->SEL0 &= ~(BIT0| BIT1 | BIT2);
    P2->SEL1 &= ~(BIT0| BIT1 | BIT2);

    /* set Port 2.0, 2.1 and 2.2 as output */
    P2->DIR |= (BIT0 | BIT1 | BIT2);

    /* clear the pins */
    P2->OUT &= ~(BIT0| BIT1 | BIT2);
}

/* capture numver of overflows */
static int overflow_counter = 0;

/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    configurePorts();

    /* configure the clock source of timer, use SMCLK = 3 MHz */
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
    /* set the operation mode of the timer */
    TIMER_A0->CTL |= TIMER_A_CTL_MC__CONTINUOUS;
    /* use a prescaler to divide the clock by 8 */
    TIMER_A0->CTL |= TIMER_A_CTL_ID_3;
    /* we have a timer running at 6 MHz */

    /* enable overflow interrupt */
    TIMER_A0->CTL |= TIMER_A_CTL_IE;

    /* enable IRQ 9 line for the timer interrupts to catch overflow */
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);

    while(1){
        __sleep();
    }

}


/* it will be called when TA0CCR0 CCIFG is set */
void TA0_N_IRQHandler(){
    switch(TA0IV){
        case 0xE:
            /* Clear timer overflow flag */
            TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
            if(++overflow_counter == 6){
                /* toogle LED */
                P2->OUT ^= BIT0;
                /* clear the variable */
                overflow_counter = 0;
            }
        break;

    }
}
