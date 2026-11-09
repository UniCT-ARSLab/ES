#include <stm32_unict_lib.h>
#include <stdio.h>
#include <string.h>

#define RTC_ADDRESS     (0b1101000 << 1)

typedef struct {
    int day, month, year;
    int hour, min, sec;
} t_time;

void setup()
{
    ClockConfig();
    CONSOLE_init();
    I2C_init(I2C1, 100000);
    I2C_write_register(I2C1, RTC_ADDRESS, 1, 0x38);
    I2C_write_register(I2C1, RTC_ADDRESS, 2, 0x09);
}

int bcd_to_integer(uint8_t byte)
{
    int tens = byte >> 4;
    int units = byte & 0b1111;
    return tens * 10 + units;
}

void loop()
{
    uint8_t data[7];
    int i;
    t_time current_time;
    I2C_read_buffer(I2C1, RTC_ADDRESS, 0, data, 7);
    // for (i = 0; i < 7;i++)
    //     printf("%02x ", data[i]);
    // printf("\n");
    current_time.sec = bcd_to_integer(data[0]);
    current_time.min = bcd_to_integer(data[1]);
    current_time.hour = bcd_to_integer(data[2]);
    current_time.day = bcd_to_integer(data[4]);
    current_time.month = bcd_to_integer(data[5]);
    current_time.year = bcd_to_integer(data[6]);

    printf("%02d/%02d/%02d - %02d:%02d:%02d\n",
            current_time.day,
            current_time.month,
            current_time.year,
            current_time.hour,
            current_time.min,
            current_time.sec);

    delay_ms(500);
}

int main()
{
    setup();
    for(;;) {
        loop();
    }
}

