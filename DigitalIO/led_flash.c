

#include "stm32_unict_lib.h"

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
}
void loop(void)
{
    GPIO_write(GPIOB, 0, 1);
    delay_ms(500);
    GPIO_write(GPIOB, 0, 0);
    delay_ms(500);
}
// set PB0 to 1
// wait 0.5 secs
// set PB0 to 0
// wait 0.5 secs

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
