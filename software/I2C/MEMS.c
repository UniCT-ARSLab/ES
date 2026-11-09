
#include <string.h>
#include <stdio.h>
#include "stm32_unict_lib.h"

typedef struct {
    int16_t x,y,z;
} t_3_axis;


typedef struct {
    t_3_axis a,g;
} t_imu_data;


#define LSM6DSO_ADDR 0xd6
#define ACC_4G  0.000122
#define GYRO_500DPS     0.01750

void LSM6_init(void)
{
    // Acc @ 208 Hz
    // Acc @ +/- 4G
    I2C_write_register(I2C1, LSM6DSO_ADDR, 0x10, 0x58);

    // Gyro @ 208 Hz
    // Gyro @ +/- 500 dps
    I2C_write_register(I2C1, LSM6DSO_ADDR, 0x11, 0x54);
}

int LSM6_data_ready(void)
{
    u_int8_t data;
    I2C_read_register(I2C1, LSM6DSO_ADDR, 0x1e, &data);
    return (data & 0x3) == 0x3;
}

void LSM6_read(t_imu_data * imu)
{
    int16_t data[6];
    I2C_read_buffer(I2C1, LSM6DSO_ADDR, 0x22, (uint8_t*)data, 12);
    imu->g.x = data[0];
    imu->g.y = data[1];
    imu->g.z = data[2];
    imu->a.x = data[3];
    imu->a.y = data[4];
    imu->a.z = data[5];
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
    LSM6_init();
}

void loop(void)
{
    t_imu_data imu;
    while (!LSM6_data_ready()) {};
    LSM6_read(&imu);
    //printf("%5d,%5d,%5d\n", imu.a.x, imu.a.y, imu.a.z);
    //printf("%10.5f,%10.5f,%10.5f\n", imu.a.x * ACC_4G, imu.a.y * ACC_4G, imu.a.z * ACC_4G);
    printf("%10.5f,%10.5f,%10.5f\n", imu.g.x * GYRO_500DPS, imu.g.y * GYRO_500DPS, imu.g.z * GYRO_500DPS);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
