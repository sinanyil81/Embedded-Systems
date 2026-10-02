#include "msp.h"

void configurePorts(){
    /* configure P2.1 and P2.2 as GPIO */
    P2->SEL0 &= ~(BIT1 | BIT2);
    P2->SEL1 &= ~(BIT1 | BIT2);

    /* set Port 2.1 and 2.2 as output */
    P2->DIR |= (BIT1 | BIT2);

    /* clear the pins */
    P2->OUT &= ~(BIT1 | BIT2);

    /* set P1.1 and P1.4 as GPIO configuration */
    P1->SEL0 &= ~(BIT1 | BIT4);
    P1->SEL1 &= ~(BIT1 | BIT4);

    /* set P1.1 and P1.4 as input pin */
    P1->DIR &= ~(BIT1 | BIT4);

    /* pullup mode configuration */
    P1->OUT |=  (BIT1 | BIT4);
    /* enable pullup resistor */
    P1->REN  |= (BIT1 | BIT4);
}

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	configurePorts();

	while (1){
	    /* catch button press */
	    while( P1->IN & BIT1);
	    /* toggle Green LED */
	    P2->OUT ^= BIT1;
	    /* catch button release */
	    while( !(P1->IN & BIT1));
	    /* toggle Green LED */
	    P2->OUT ^= BIT1;

	    /* catch button press */
	    while( P1->IN & BIT4);
	    /* toggle Green LED */
	    P2->OUT ^= BIT2;
	    /* catch button release */
	    while( !(P1->IN & BIT4));
	    /* toggle Green LED */
	    P2->OUT ^= BIT2;

	}

}
