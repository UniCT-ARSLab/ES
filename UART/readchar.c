

#include "stm32_unict_lib.h"
#include <stdlib.h>

void setup(void)
{
    ClockConfig();

    CONSOLE_init();
}

void loop(void)
{
    int c = readchar();
    printf("Il carattere digitato ha codice = %x (%c) \n", c, c);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
