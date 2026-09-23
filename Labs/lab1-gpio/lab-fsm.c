#include "msp.h"

void configurePorts(){
    /* configure P2.0, P2.1 and P2.2 as GPIO */
    P2->SEL0 &= ~(BIT0| BIT1 | BIT2);
    P2->SEL1 &= ~(BIT0| BIT1 | BIT2);

    /* set Port 2.0, 2.1 and 2.2 as output */
    P2->DIR |= (BIT0 | BIT1 | BIT2);

    /* clear the pins */
    P2->OUT &= ~(BIT0| BIT1 | BIT2);

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

/* catch S1 event */
void checkS1(){
    /* catch button press */
    while(...);
    /* catch button release */
    while(...);
}

/* catch S2 event */
void checkS2(){
    /* catch button press */
    while(...);
    /* catch button release */
    while(...);
}

void RedOn(){
    /* clear all the pins */
    ...
    /* turn on RED */
    ...
}

void GreenOn(){
    /* clear all the pins */
    ...
    /* turn on GREEN */
    ...
}

void BlueOn(){
    /* clear all the pins */
    ...
    /* turn on BLUE */
    ...
}

typedef enum {
    STATE_INIT,
    STATE_GREEN,
    STATE_RED,
    STATE_BLUE,
    NUM_STATES
}State_t;

typedef struct{
    State_t state;
    void (*state_function)(void);
} StateMachine_t;

void fn_INIT(void);
void fn_GREEN(void);
void fn_RED(void);
void fn_BLUE(void);

State_t current_state = STATE_INIT;

StateMachine_t fsm[] = {
                      {...},
                      {...},
                      {...},
                      {...}
};

void fn_INIT(){
    checkS1();
    current_state = STATE_GREEN;
}

void fn_GREEN(){
    GreenOn();
    checkS2();
    current_state = STATE_RED;
}

void fn_RED(){
    RedOn();
    checkS1();
    current_state = STATE_BLUE;
}

void fn_BLUE(){
    BlueOn();
    checkS2();
    current_state = STATE_GREEN;
}


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	configurePorts();

	while(1){
	    if(current_state < NUM_STATES){
            /* execute current state */
	        ...
	    }
	    else{
	        /* serious error */
	    }

	}
}
