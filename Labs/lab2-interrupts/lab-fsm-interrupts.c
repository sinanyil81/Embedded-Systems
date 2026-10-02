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

void configureInterrupts(){
    /* P1.1 and P1.4 as high-to-low transition interrupt */
    ...
    /* enable P1.1 and P1.4 interrupt */
    ...
    /* clear interrupt flag */
    ...
    /* play with ISER1 register to set IRQ35 -> P1 */
    ...
}

void RedOn(){
    /* clear all the pins */
    P2->OUT &= ~(BIT0| BIT1 | BIT2);
    /* turn on RED */
    P2->OUT |= BIT0;
}

void GreenOn(){
    /* clear all the pins */
    P2->OUT &= ~(BIT0| BIT1 | BIT2);
    /* turn on GREEN */
    P2->OUT |= BIT1;
}

void BlueOn(){
    /* clear all the pins */
    P2->OUT &= ~(BIT0| BIT1 | BIT2);
    /* turn on BLUE */
    P2->OUT |= BIT2;
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
                      {STATE_INIT, fn_INIT},
                      {STATE_GREEN, fn_GREEN},
                      {STATE_RED, fn_RED},
                      {STATE_BLUE, fn_BLUE}
};

typedef enum {
    SW_NONE,
    SW1_PRESSED,
    SW2_PRESSED
} Event_t;

Event_t event = SW_NONE;

void fn_INIT(){
    if (event == SW1_PRESSED){
        GreenOn();
        current_state = STATE_GREEN;
    }
}

void fn_GREEN(){
    if (event == SW2_PRESSED){
        RedOn();
        current_state = ...;
    }
}

void fn_RED(){
    if(event ==SW1_PRESSED){
        BlueOn();
        current_state = ...;
    }
}

void fn_BLUE(){
    if(event == SW2_PRESSED){
        GreenOn();
        current_state = ...;
    }
}


/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    configurePorts();
    configureInterrupts();

    while(1){
        /* Put MCU into LPM0 power mode -> consume less power */
        __sleep();
        if(current_state < NUM_STATES){
            (*fsm[current_state].state_function)();
        }
        else{
            /* serious error */
        }

    }
}


/* ISR Handler for PORT1 */
void PORT1_IRQHandler(){

    /* check if there is a pending interrupt on P1.1 */
    if(P1->IFG & BIT1){
        event = SW1_PRESSED;
        /* clear the interrupt flags */
        P1->IFG &= ~BIT1;

    }
    else if (P1->IFG & BIT4){
        event = ...;
        /* clear the interrupt flags */
        P1->IFG &= ~BIT4;
    }
}
