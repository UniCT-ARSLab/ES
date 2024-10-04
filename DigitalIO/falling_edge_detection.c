

#include "stm32_unict_lib.h"

int prev_state;

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_input(GPIOB, 10); // configure pin PB10 as input
    prev_state = GPIO_read(GPIOB, 10); // read PB10
}

void loop(void)
{
    int current_state = GPIO_read(GPIOB, 10); // read PB10
    if (prev_state == 1 && current_state == 0) {
        // falling edge detected
        GPIO_toggle(GPIOB, 0);
    }
    prev_state = current_state;
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
