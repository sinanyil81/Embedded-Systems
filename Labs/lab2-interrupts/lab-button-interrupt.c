#include "msp.h"

void configurePorts(){
    /* configure P2.0, P2.1 and P2.2 as GPIO */
    P2->SEL0 &= ~(BIT0| BIT1 | BIT2);
    P2->SEL1 &= ~(BIT0| BIT1 | BIT2);

    /* set Port 2.0, 2.1 and 2.2 as output */
    P2->DIR |= (BIT0 | BIT1 | BIT2);

    /* clear the pins */
    P2->OUT &= ~(BIT0| BIT1 | BIT2);

    /* set P1.1 as GPIO configuration */
    P1->SEL0 &= ~BIT1;
    P1->SEL1 &= ~BIT1;

    /* set P1.1 and P1.4 as input pins */
    P1->DIR &= ~(BIT1 | BIT4);
    /* pullup mode configuration */
    P1->OUT |=  (BIT1 | BIT4);
    /* enable pullup resistor */
    P1->REN  |= (BIT1 |BIT4);
}


/**
 * main.c
 */
void main(void)
{
    int i =0;
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	configurePorts();

	/* P1.1 and P1.4 as high-to-low transition interrupt */
	...
	/* enable P1.1 and P1.4 interrupt */
	...
	/* clear interrupt flag */
	P1->IFG = 0;
	/* play with ISER1 register to set IRQ35 -> P1 */
	NVIC->ISER[1] |= 1 << (PORT1_IRQn & 31);

	while(1){
	    /* Put MCU into LPM0 power mode -> consume less power */
	    __sleep();
	    i++;
	}

}

/* ISR Handler for PORT1 */
void PORT1_IRQHandler(){

    /* check if there is a pending interrupt on P1.1 */
    if(P1->IFG & BIT1){
        /* toggle all red LED */
        P2->OUT ^= BIT0;
    }

    /* check if there is a pending interrupt on P1.4 */
    if(...){
        /* toggle all green LED */
        ...
    }

    /* clear the interrupt flags */
    P1->IFG &= ~(BIT1 | BIT4);
}
