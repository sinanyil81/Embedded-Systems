#include "msp.h"


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    /* configure Port1 GPIO operation */
    P1->SEL0=0;
    P1->SEL1=0;

    P1->DIR =~BIT1; /* configure P1.1 as input */

    /* pull-up resistor mode enable */
    P1->OUT = BIT1;
    P1->REN = BIT1;

    while(1){
        /* while the button is not pressed */
        while(P1->IN & BIT1);
        /* while the button is pressed */
        while(!(P1->IN & BIT1));
    }
}
