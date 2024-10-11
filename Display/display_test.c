
#include <string.h>
#include <stdio.h>
#include "stm32_unict_lib.h"

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    DISPLAY_init();
}

int i = 0;

void loop(void)
{
    char s[5];
    sprintf(s,"%4d",i);
    DISPLAY_puts(0, s);
    delay_ms(200);
    i++;
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
