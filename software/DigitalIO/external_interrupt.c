

#include "stm32_unict_lib.h"

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 3); // configure pin PB3 as output
    GPIO_config_input(GPIOB, 4); // configure pin PB4 as input
    GPIO_config_EXTI(GPIOB, EXTI4); // attach PB4 to EXTI4
    EXTI_enable(EXTI4, FALLING_EDGE); // detect falling edge con EXTI4
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        GPIO_toggle(GPIOB, 0); // change state to the LED
        EXTI_clear(EXTI4);
    }
}

void loop(void)
{
    delay_ms(200);
    GPIO_toggle(GPIOC, 3);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
