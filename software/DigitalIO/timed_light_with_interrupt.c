#include "stm32_unict_lib.h"

enum {
    st_OFF,
    st_ON,
    st_ALWAYS_ON
};
volatile int state = st_OFF;
volatile int seconds_count = 0;

void setup()
{
    ClockConfig();
    GPIO_init(GPIOB);
    GPIO_init(GPIOC);
    GPIO_config_input(GPIOB, 4);
    GPIO_config_input(GPIOB, 10);
    GPIO_config_input(GPIOB, 5);
    GPIO_config_input(GPIOB, 6);
    GPIO_config_output(GPIOB, 0);
    GPIO_config_output(GPIOC, 2);

    GPIO_config_EXTI(GPIOB, EXTI4);
    EXTI_enable(EXTI4, FALLING_EDGE);

    GPIO_config_EXTI(GPIOB, EXTI10);
    EXTI_enable(EXTI10, FALLING_EDGE);

    GPIO_config_EXTI(GPIOB, EXTI5);
    EXTI_enable(EXTI5, FALLING_EDGE);

    GPIO_config_EXTI(GPIOB, EXTI6);
    EXTI_enable(EXTI6, FALLING_EDGE);
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        state = st_ALWAYS_ON;
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        EXTI_clear(EXTI6);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {
        state = st_OFF;
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        state = st_ON;
        seconds_count = 0;
        EXTI_clear(EXTI4);
    }
}

void loop()
{
    switch (state)
    {
    case st_OFF:
        GPIO_write(GPIOB, 0, 0);
        GPIO_write(GPIOC, 2, 0);
        break;
    
    case st_ON:
        GPIO_write(GPIOB, 0, 1);  
        delay_ms(250);

        ++seconds_count;
        if (seconds_count >= 15*4) {
            GPIO_toggle(GPIOC,2);
        }
        else
            GPIO_write(GPIOC,2,0);
        if (seconds_count == 20*4)
            state = st_OFF;
        break;

    case st_ALWAYS_ON:
        GPIO_write(GPIOB, 0, 1);
        break;  
    }
}

int main()
{
    setup();
    for(;;)
        loop();
}
