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

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    configurePorts();

    /* configure the clock source of timer, use ACLK = 32 KHz */
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__ACLK;
    /* select up counting mode -> timer repeatedly counts from zero to TA0CCR0 */
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    /* each tick is approximately 32 microsecond */

    /* timer will count up to this value */
    TIMER_A0->CCR[0]= 9000;
    /* the compare value for the second timer */
    TIMER_A0->CCR[1]= 3000;

    /* enable interrupt so that we can capture the event when TACCRx = CCRx */
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;

    /* enable IRQ for TIMERA interrupt on NVIC */
    NVIC->ISER[0] = 1 << (... & 31);
    NVIC->ISER[0] = 1 << (... & 31);

    while(1){
        __sleep();
    }

}

/* interrupt handler for all other CCR1-CCR7 events and timer overflow */
void TA0_N_IRQHandler(){

    switch(TA0IV){
        case 0x2:
            /* clear the interrupt flag */
            TIMER_A0->CCTL[1] &= (~TIMER_A_CCTLN_CCIFG);
            // toggle LED
            ...
        break;

    }
}

/* interrupt handler for CCR0 events */
void TA0_0_IRQHandler(){

    // clear the interrupt flag
    TIMER_A0->CCTL[0] &= (~TIMER_A_CCTLN_CCIFG);
    // toggle LED
    ...

}
