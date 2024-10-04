

#include "stm32_unict_lib.h"

typedef enum {
    LIGHT_OFF,
    LIGHT_ON
} t_light_state;

int prev_state;
t_light_state light_state = LIGHT_OFF;
int delay_counter;

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
        switch (light_state) {
            case LIGHT_OFF:
                light_state = LIGHT_ON;
                delay_counter = 0;
                break;
            case LIGHT_ON:
                delay_counter = 0;
                break;
        }
    }
    if (light_state == LIGHT_ON) {
        delay_ms(10);
        delay_counter++;
        if (delay_counter == 1000) {
            light_state = LIGHT_OFF;
        }
    }

    if (light_state == LIGHT_ON)
        GPIO_write(GPIOB, 0, 1);
    else
        GPIO_write(GPIOB, 0, 0);

    prev_state = current_state;

}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
