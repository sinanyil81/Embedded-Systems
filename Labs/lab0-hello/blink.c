#include "msp.h"

/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    /* configure P1.0 as output */
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;

    P1->DIR |= BIT0;

    /* Pull it up */
    P1->OUT |=BIT0;

    /* Pull it down */
    P1->OUT &=~BIT0;

    while(1);
}
