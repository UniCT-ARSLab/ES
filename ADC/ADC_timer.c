
#include <stdio.h>
#include "stm32_unict_lib.h"

int period = 500;

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    DISPLAY_init();

    GPIO_init(GPIOB);
    GPIO_config_output(GPIOB, 0);

    ADC_init(ADC1, ADC_RES_12, ADC_ALIGN_RIGHT);
    ADC_channel_config(ADC1, GPIOC, 0, 10);

    ADC_on(ADC1);
    ADC_sample_channel(ADC1, 10);

    TIM_init(TIM2);
    TIM_config_timebase(TIM2, 42000, 500); // 250 ms
    TIM_on(TIM2);
    TIM_enable_irq(TIM2, IRQ_UPDATE);
}

void TIM2_IRQHandler()
{
    if (TIM_update_check(TIM2)) {
        GPIO_toggle(GPIOB, 0);
        TIM2->ARR = period;
        TIM_update_clear(TIM2);
    }
}
void loop(void)
{
    char s[5];
    ADC_start(ADC1);
    while (!ADC_completed(ADC1)) { }
    int value = ADC_read(ADC1);
    period = (2000 * value)/4095;
    sprintf(s, "%4d", period/2);

    DISPLAY_puts(0, s);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
