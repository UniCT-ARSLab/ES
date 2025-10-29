
#include <stdio.h>
#include "stm32_unict_lib.h"

enum {
    st_IDLE,
    st_PRE_CLOSING,
    st_CLOSING,
    st_MOVE,
    st_OPENING
};

volatile int state = st_IDLE;
volatile int current_h_floor = 0;
volatile int target_h_floor;

int delay_count = 0;

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 3); // configure pin PC3 as output
    GPIO_config_output(GPIOC, 2); // configure pin PC2 as output

    GPIO_config_input(GPIOB, 10); // configure pin PB10 as input
    GPIO_config_input(GPIOB, 4); // configure pin PB4 as input
    GPIO_config_input(GPIOB, 5); // configure pin PB5 as input
    GPIO_config_input(GPIOB, 6); // configure pin PB6 as input
   
    GPIO_config_EXTI(GPIOB, EXTI10); // attach PB10 to EXTI10
    GPIO_config_EXTI(GPIOB, EXTI4); // attach PB4 to EXTI4
    GPIO_config_EXTI(GPIOB, EXTI5); // attach PB5 to EXTI5
    GPIO_config_EXTI(GPIOB, EXTI6); // attach PB6 to EXTI6

    EXTI_enable(EXTI10, FALLING_EDGE);
    EXTI_enable(EXTI4, FALLING_EDGE);
    EXTI_enable(EXTI5, FALLING_EDGE);
    EXTI_enable(EXTI6, FALLING_EDGE);

    DISPLAY_init();
}

void go_to_floor(int target)
{
    target_h_floor = target;
    state = st_PRE_CLOSING;
    delay_count = 0;
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {
        // pushbutton X
        go_to_floor(6);
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // pushbutton Y
        go_to_floor(4);
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        // pushbutton Z
        go_to_floor(2);
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        // pushbutton T
        go_to_floor(0);
        EXTI_clear(EXTI6);
    }
}

void loop(void)
{
    char s[10];
    delay_ms(250);
    if ((current_h_floor % 2) == 0)
        sprintf(s, "%d ", current_h_floor / 2);
    else
        sprintf(s, "%d-", current_h_floor / 2);    
    DISPLAY_puts(0, s);
    switch (state)
    {
    case st_PRE_CLOSING:
        ++delay_count;
        if (delay_count == 4) {
            state = st_CLOSING;
            delay_count = 0;
        }
        break;
    case st_CLOSING:
        ++delay_count;
        GPIO_toggle(GPIOC, 2);
        if (delay_count == 6) {
            state = st_MOVE;
            delay_count = 0;
        }
        break;
    case st_MOVE:
        ++delay_count;
        GPIO_toggle(GPIOB, 0);
        if (delay_count == 4) {
            delay_count = 0;
            if (target_h_floor == current_h_floor)
                state = st_OPENING;
            else if (target_h_floor > current_h_floor)
                ++current_h_floor;
            else
                --current_h_floor;
        }
        break;
    case st_OPENING:
        ++delay_count;
        GPIO_toggle(GPIOC, 3);
        if (delay_count == 6) {
            state = st_IDLE;
            delay_count = 0;
        }
        break;        
    default:
        break;
    }
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
