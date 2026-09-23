#include "msp.h"


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    /* configure Port1 GPIO operation */
    ...

    /* configure P1.1 as input */
    ...

    /* pull-up resistor mode enable */
    ...

    while(1){
        /* while the button is not pressed */
        while(...);
        /* toggle LED */
        ...
        /* while the button is pressed */
        while(...);
        /* toggle LED */
        ...
    }
}
