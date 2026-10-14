#include "msp.h"


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    /* set direction of the port 1 pin 0*/
    P1->DIR|=BIT0;
    /* set direction of the port2  pin 0, 1 and 2 */
    P2->DIR |= BIT0|BIT1|BIT2;


    /* play with the MCLK -> setting main CPU clock freq. to 3 MHZ */
    CS->KEY = CS_KEY_VAL;
    CS->CTL1 = CS_CTL1_SELM_3 | CS_CTL1_DIVM_7; /* divide signal by 8 */;
    CS->KEY = 0;

    while (1)                               // continuous loop
    {
        int i;
            P1->OUT ^= BIT0;                    // Blink P1.0 LED
            P2->OUT ^= BIT0;
            P2->OUT ^= BIT1;
            P2->OUT ^= BIT2;
            for (i = 20000; i > 0; i--);        // Delay
    }
}
