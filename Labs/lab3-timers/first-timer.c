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

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	configurePorts();

	/* configure the clock source of timer, use SMCLK = 3 MhZ */
	TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
	/* set the operation mode of the timer */
	TIMER_A0->CTL |= TIMER_A_CTL_MC__CONTINUOUS;
	/* use a prescaler to divide the clock by 8 */
	TIMER_A0->CTL |= TIMER_A_CTL_ID_3;

	/* configure CCR0 hardware */
	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
	/* load the max value */
	TIMER_A0->CCR[0] = 50000;

	/* enable IRQ line for the timer */
	NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

	while(1){
	    __sleep();
	}

}


/* it will be called when TA0CCR0 CCIFG is set */
void TA0_0_IRQHandler(){

   /* clear the interrupt flag */
   TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

   // toggle red LED
   P2->OUT ^= BIT0;
}
