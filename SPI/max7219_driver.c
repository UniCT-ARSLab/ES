
#include <string.h>
#include <stdio.h>
#include "stm32_unict_lib.h"

#define CS(v)   GPIO_write(GPIOC,1,v)
#define DIN(v)   GPIO_write(GPIOB,0,v)
#define CLK(v)   GPIO_write(GPIOC,0,v)

void MAX7219_init()
{
    GPIO_init(GPIOB);
    GPIO_init(GPIOC);
    GPIO_config_output(GPIOB,0);
    GPIO_config_output(GPIOC,0);
    GPIO_config_output(GPIOC,1);
    CS(1);
    CLK(0);
}

void MAX7219_send(uint16_t v)
{
    int i;
    CS(0);
    //delay_ms(1);
    for (i = 0;i < 16;i++) {
        // 0x8000 = 1000 0000 0000 0000
        if ( (v & 0x8000) == 0)
            DIN(0);
        else   
            DIN(1);
        //delay_ms(1);
        CLK(1);
        //delay_ms(1);
        CLK(0);
        //delay_ms(1);
        v = v << 1;
    }
    CS(1);
    delay_ms(1);
}

void setup(void)
{
    int i;
    // configure the clock @ 84 MHz
    ClockConfig();
    MAX7219_init();
    //
    MAX7219_send(0x0900); // reg9 = no decode
    MAX7219_send(0x0A0F); // regA = max intensity
    MAX7219_send(0x0B07); // regB = scan all digits
    MAX7219_send(0x0C01); // regC = normal mode

    for (i = 1; i <= 8;i++)
        MAX7219_send( (i << 8) | 0);
}

uint8_t display_buffer[8] = { 0,0,0,0,0,0,0,0};

void update()
{
    int i;
    for (i = 1; i <= 8;i++)
        MAX7219_send( (i << 8) | display_buffer[i-1]);
}

// row = 0-7, col = 0-7
void _put_pixel(int row, int col, int p) 
{
    uint8_t row_value = display_buffer[row];

    if (p == 0) {
        // impostare bit "col" = 0
        row_value &= ~(1 << (7-col));
    }
    else {
        // impostare bit "col" = 1
        row_value |= (1 << (7-col));
    }
    display_buffer[row] = row_value;
}

void put_pixel(int row, int col, int p) 
{
    _put_pixel(row, col, p);
    update();
}

void put_rectangle(int row, int col, int w, int h)
{
    int i;
    for (i = 0; i < w; i++) {
        _put_pixel(row, col+i, 1);
        _put_pixel(row + h - 1, col+i, 1);
    }
    for (i = 0; i < h; i++) {
        _put_pixel(row + i, col, 1);
        _put_pixel(row + i, col+w - 1, 1);
    }
    update();
}

void loop(void)
{
    // diagonale
    // put_pixel(0,0,1);
    // put_pixel(1,1,1);
    // put_pixel(2,2,1);
    // put_pixel(3,3,1);
    // put_pixel(4,4,1);
    // put_pixel(5,5,1);
    // put_pixel(6,6,1);
    // put_pixel(7,7,1);
    put_rectangle(3,2,4,3);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
