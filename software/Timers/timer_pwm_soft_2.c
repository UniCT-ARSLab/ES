

#include "stm32_unict_lib.h"

int T_on, T_off;

int LED_on = 0;

#define PERIOD 2000

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    DISPLAY_init();
    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
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

    T_on = PERIOD / 2;
    T_off = PERIOD / 2;

	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 420, T_off);
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);

}

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
        if (LED_on)
		    GPIO_toggle(GPIOB,0);
        else
		    GPIO_write(GPIOB,0, 0);
        if (GPIO_read(GPIOB,0) == 1) {
            // stato on
            TIM2->ARR = T_on;
        }        
        else {
            // stato off
            TIM2->ARR = T_off;
        }
		TIM_update_clear(TIM2);
	}
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {       
        // X, decrement 
        if (T_on > 20 && LED_on) {
            T_on -= 10;
            T_off = PERIOD - T_on;
        }
         EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // Y, increment
        if (T_on < PERIOD - 20 && LED_on) {
            T_on += 10;
            T_off = PERIOD - T_on;
        }
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        // Z on/off
        LED_on = !LED_on;
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        EXTI_clear(EXTI6);
    }
}

void loop(void)
{
    char s[5];
    sprintf(s, "%4d", T_on);
    DISPLAY_puts(0, s);   
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
