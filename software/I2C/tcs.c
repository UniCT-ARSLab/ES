
#include <string.h>
#include <stdio.h>
#include "stm32_unict_lib.h"

#define TCS_ADDR 0x52

void TCS_init(void)
{
    I2C_write_register(I2C1, TCS_ADDR, 0x81, 0xc0);
}

void TCS_read(uint16_t * colors)
{
    I2C_write_register(I2C1, TCS_ADDR, 0x80, 0x03);
    delay_ms(200);
    // 1011 0110
    I2C_read_buffer(I2C1, TCS_ADDR, 0xb6,colors,6);
    I2C_write_register(I2C1, TCS_ADDR, 0x80, 0x13);
}

void setup(void)
{
    // configure the clock @ 84 MHz
    ClockConfig();
    CONSOLE_init();
    //
    GPIO_init(GPIOB);
    GPIO_config_alternate(GPIOB, 8, AF_I2C1_3); // PB8 as SCL
    GPIO_config_alternate(GPIOB, 9, AF_I2C1_3); // PB9 as SDA
    I2C_init(I2C1,400000);
    //
    TCS_init();
}

void loop(void)
{
    uint16_t channels[3];
    TCS_read(channels);
    printf("%d,%d,%d\n", channels[0], channels[1], channels[2]);
    delay_ms(500);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
