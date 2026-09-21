#include "msp.h"

void configurePorts(){
    /* configure P1.0 as output */
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;

    /* configure P2.0, P2.1 and P2.2 as output */
    P2->SEL0 &= ~(BIT0 | BIT1 | BIT2);
    P2->SEL1 &= ~(BIT0 | BIT1 | BIT2);

    /* set Port 1 pin 0 as output */
    P1->DIR |= BIT0;

    /* set Port 2 pins 0,1 and 2 as output */
    P2->DIR |= (BIT0 | BIT1 | BIT2);
}

/**
 * main.c
 */
void main(void)
{
    int i = 0;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	configurePorts();

	while(1){
	    /* toggle ports */
	    P1->OUT ^=BIT0;
	    P2->OUT ^= BIT0;
	    P2->OUT ^= BIT1;
	    P2->OUT ^= BIT2;
	    /* delay a little bit */
	    for (i=0;i<100000;i++);
	}
}
