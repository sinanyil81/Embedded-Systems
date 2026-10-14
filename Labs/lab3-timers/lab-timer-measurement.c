#include "msp.h"


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    /* set direction of the port 1 pin 0*/
    P1->DIR|=BIT0;
    /* set direction of the port2  pin 0, 1 and 2 */
    P2->DIR |= BIT0|BIT1|BIT2;


    /* play with the MCLK -> setting main CPU clock freq. to 3 MHZ */
    CS->KEY = CS_KEY_VAL;
    CS->CTL1 = CS_CTL1_SELM_3;
    CS->KEY = 0;

    /* Select clock source as ACLK = 32KHz. This means that the timer will have 32768
     * ticks per second. Or in other words, each tick will be approximately 30 microseconds.
     * Since the timer register is 16-bits, the maximum value of it will be 65535. This means
     * that, one timer wrap-around (overflow) will correspond to 30 X 65535 = approximately 2
     * seconds. Or in other words, in every 2 seconds, the timer will overflow. */
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__ACLK;


    /* continuous loop */
    while (1) {
        int i;
        int value;
        /* blin LEDs */
        P1->OUT ^= BIT0;
        P2->OUT ^= BIT0;
        P2->OUT ^= BIT1;
        P2->OUT ^= BIT2;

        /* clear timer register */
        TIMER_A0->R = 0;
        value = 0;
        /* select continuous counting mode */
        TIMER_A0->CTL |= TIMER_A_CTL_MC__CONTINUOUS;

        /* create a delay */
        for (i = 20000; i > 0; i--);

        /* Read the value stored in timer. This value might not represent the actual time passed if
         * the loop finishes in more than 2 seconds since the timer will overflow. */
        value = ...;
        /* stop timer */
        TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;
    }
}
