

#include "stm32_unict_lib.h"

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_input(GPIOB, 10); // configure pin PB10 as input
}
void loop(void)
{
    int pushbutton = GPIO_read(GPIOB, 10); // read PB10
    GPIO_write(GPIOB, 0, !pushbutton); // write PB0
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
