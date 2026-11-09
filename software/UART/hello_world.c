

#include "stm32_unict_lib.h"
#include <stdlib.h>

void setup(void)
{
    ClockConfig();

    CONSOLE_init();
}

int i = 0;

void loop(void)
{
    printf("Hello world, %d\n", i);
    delay_ms(1000);
    i++;
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
