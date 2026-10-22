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

}

void configureTimer(){
    /* configure the clock source of timer, use SMCLK = 3 MHz */
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
    /* set the operation mode of the timer */
    TIMER_A0->CTL |= TIMER_A_CTL_MC__CONTINUOUS;
    /* use a prescaler to divide the clock by 8 */
    TIMER_A0->CTL |= TIMER_A_CTL_ID_3;
    /* we have a timer running at 6 MHz */

    /* enable overflow interrupt */
    TIMER_A0->CTL |= TIMER_A_CTL_IE;

    /* enable IRQ 9 line for the timer interrupts to catch overflow */
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
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

void fn_INIT(){
       GreenOn();
        current_state = STATE_GREEN;
}

void fn_GREEN(){
        RedOn();
        current_state = STATE_RED;
}

void fn_RED(){
        BlueOn();
        current_state = STATE_BLUE;
}

void fn_BLUE(){
        GreenOn();
        current_state = STATE_GREEN;
}

static int one_sec_passed = 0;

/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    configurePorts();
    configureTimer();

    while(1){
        /* Put MCU into LPM0 power mode -> consume less power */
        __sleep();
        if(one_sec_passed){
            if(current_state < NUM_STATES){
                (*fsm[current_state].state_function)();
            }
            else{
                /* serious error */
            }

            one_sec_passed = 0;
        }
    }
}

/* capture numver of overflows */
static int overflow_counter = 0;

/* it will be called when TA0CCR0 CCIFG is set */
void TA0_N_IRQHandler(){
    switch(TA0IV){
        case 0xE:
            /* Clear timer overflow flag */
            TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
            if(++overflow_counter == 6){
                /* clear the variable */
                overflow_counter = 0;
                /* notify that 1 second has passed */
                one_sec_passed = 1;
            }
        break;

    }
}
