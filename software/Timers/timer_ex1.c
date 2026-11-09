#include "stm32_unict_lib.h"
#include <stdio.h>

void setup(void)
{
    ClockConfig();

	DISPLAY_init();

	GPIO_init(GPIOB);
	GPIO_config_output(GPIOB, 0);

	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 42000, 2000);
    // increment frequency = 84e6 / 42e3 = 2000 Hz
    // autoreload = 2000 --> update every second
	TIM_on(TIM2);
}

void loop(void)
{
	char s[6];

	sprintf(s, "%4d", TIM_get(TIM2));
	DISPLAY_puts(0,s);

	if (TIM_update_check(TIM2)) {
        // update every second
		GPIO_toggle(GPIOB,0);
		TIM_update_clear(TIM2);
	}

}

int main()
{
	setup();
	for(;;) {
		loop();
	}
	return 0;
}
