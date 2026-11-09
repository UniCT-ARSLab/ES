

#include "stm32_unict_lib.h"
#include <string.h>

typedef enum {
    st_IDLE,
    st_OPENING,
    st_WAIT,
    st_CLOSING
} t_state;

t_state state = st_IDLE;
int percentage = 0;
int wait_counter = 0;
int flash_counter = 0;

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz

    DISPLAY_init();
    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 3); // configure pin PB3 as output

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

    TIM_init(TIM2);
	TIM_config_timebase(TIM2, 42000, 200);
    // increment frequency = 84e6 / 42e3 = 2000 Hz
    // autoreload = 200 --> update every 1/10th second
	TIM_enable_irq(TIM2, IRQ_UPDATE);

	TIM_on(TIM2);

}

void led_flash(void)
{
    ++flash_counter;
    if (flash_counter >= 5) {
        GPIO_toggle(GPIOB, 0);
        flash_counter = 0;
    }
}

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
        // update every 1/10th second
        switch (state) {
            case st_OPENING:
                percentage += 1;
                if (percentage == 100) {
                    state = st_WAIT;
                }
                led_flash();
                break;
            case st_CLOSING:
                percentage -= 1;
                if (percentage == 0) {
                    state = st_IDLE;
                }
                led_flash();
                break;
            case st_WAIT:
                ++wait_counter;
                if (wait_counter == 40) { // 4 seconds
                    wait_counter = 0;
                    state = st_CLOSING;
                }
                led_flash();
                break;
            default:
                GPIO_write(GPIOB, 0, 0);
        }
		TIM_update_clear(TIM2);
	}
}


void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {
        // pushbutton X
        if (state == st_IDLE || state == st_CLOSING) {
            state = st_OPENING;
        }
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // pushbutton Y
        if (state == st_OPENING || state == st_WAIT) {
            state = st_CLOSING;
        }
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        // pushbutton Z
        switch (state) {
            case st_WAIT:
                wait_counter = 0;
                break;
            case st_CLOSING:
                state = st_OPENING;
                break;
        }
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        // pushbutton T
        EXTI_clear(EXTI6);
    }
}

void loop(void)
{
    char s[5];
    if (percentage < 25)
        strcpy(s,"----");
    else if (percentage < 50)
        strcpy(s,"--- ");
    else if (percentage < 75)
        strcpy(s,"--  ");
    else if (percentage < 100)
        strcpy(s,"-   ");
    else
        strcpy(s,"    ");
    DISPLAY_puts(0, s);   
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
