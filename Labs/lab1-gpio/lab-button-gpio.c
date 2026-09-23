#include "msp.h"

void configurePorts(){
    /* configure P2.1 and P2.2 as GPIO */
    ...

    /* set Port 2.1 and 2.2 as output */
    ...

    /* clear the pins */
    ...

    /* set P1.1 and P1.4 as GPIO configuration */
    ...

    /* set P1.1 and P1.4 as input pin */
    ...

    /* pullup mode configuration */
    ...
    /* enable pullup resistor */
    ...
}

/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	configurePorts();

	while (1){
	    /* catch button 1 press */
	    while(...);
	    /* toggle Green LED */
	    ...
	    /* catch button 1 release */
	    while(...);
	    /* toggle Green LED */
	    ...

	    /* catch button 2 press */
	    while(...);
	    /* toggle Blue LED */
	    ...
	    /* catch button 2 release */
	    ...
	    /* toggle Blue LED */
	    ...
	}

}
