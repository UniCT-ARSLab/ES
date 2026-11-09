

#include "stm32_unict_lib.h"
#include <stdlib.h>

volatile int measure_done = 0;
volatile int timeout = 0;

void setup(void)
{
    ClockConfig();

    DISPLAY_init();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_alternate(GPIOB, 4, 2); // configure pin PB4 as AF2
   
	TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 50000);
    // increment frequency = 84e6 / 84e2 = 10000 Hz (time tick 100 microsecs)
    // autoreload = 50000 --> update every 5 sec
    TIM_config_IC(TIM3, 1, IC_FALLING_EDGE); // capture on falling-edge (pushbutton)
	TIM_enable_irq(TIM3, IRQ_UPDATE | IRQ_CC_1);
    TIM_set(TIM3,0);
	TIM_on(TIM3);

}

void TIM3_IRQHandler(void)
{
    if (TIM_update_check(TIM3)) {
        // period match, timeout
        GPIO_write(GPIOB, 0, 0);
        timeout = 1;
        TIM_update_clear(TIM3);
    }

    if (TIM_cc_check(TIM3,1)) {
        // capture
        GPIO_write(GPIOB, 0, 0);
        measure_done = 1;
        TIM_cc_clear(TIM3,1);
    }    
}

void loop(void)
{
    int t = 1000 + (rand() * 3000) / RAND_MAX; // random from 1000 to 4000 ms
    delay_ms(t); // wait t milliseconds to start the game

    GPIO_write(GPIOB,0,1);
    TIM_set(TIM3, 0);
    measure_done = 0;
    timeout = 0;
    DISPLAY_puts(0,"    ");

    while (!measure_done && !timeout) { }

    if (measure_done) {
        int ms = TIM3->CCR1 / 10;
        char s[5];
        sprintf(s,"%4d", ms);
        DISPLAY_puts(0, s);
    }
    else {
        DISPLAY_puts(0, "tout");
    }
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
