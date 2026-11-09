#include <stdio.h>
#include "stm32_unict_lib.h"
int new_arr_value = 100;

void main(void)
{
    ClockConfig();

    CONSOLE_init();

    ADC_init(ADC1, ADC_RES_12, ADC_ALIGN_RIGHT);
    ADC_channel_config(ADC1, GPIOA, 0, 0);
    ADC_on(ADC1);
    ADC_sample_channel(ADC1, 0);

    for (;;)
    {
        ADC_start(ADC1);
        while (!ADC_completed(ADC1))
        {
        }
        int value = ADC_read(ADC1);
        float v_adc = 3.3/4095 * value;
        float v_out = 12/3.3 * v_adc;
        float distance = 100 + 90 * v_out;
        printf("ADC = %d, V_adc = %f, V_out = %f, d(mm) = %f\n", value, v_adc, v_out, distance);
        delay_ms(100);
    }
}
