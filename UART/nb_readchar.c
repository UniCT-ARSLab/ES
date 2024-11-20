

#include "stm32_unict_lib.h"
#include <stdio.h>

void setup(void)
{
    ClockConfig();
    CONSOLE_init();
}

int i = 0;
void loop(void)
{
    printf("Hello, %d\n", i);
    i++;
    if (kbhit()) {
        readchar();
        printf("STOP!\n");
        readchar();
    }
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
