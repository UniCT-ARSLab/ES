/*
 * imu.c
 *
 *  Created on: Nov 26, 2018
 *      Author: corrado
 */

#include "stm32_unict_lib.h"
#include <stdio.h>

void activate_col(int col)
{
	GPIO_write(GPIOA, 10, col != 0);
	GPIO_write(GPIOB, 3, col != 1);
	GPIO_write(GPIOB, 5, col != 2);
	GPIO_write(GPIOB, 4, col != 3);
}

void keypad_init(void)
{
	GPIO_init(GPIOA);
	GPIO_init(GPIOB);
	GPIO_init(GPIOC);

	// cols
	// PA10, PB3, PB5, PB4
	GPIO_config_output(GPIOA, 10);
	GPIO_OPEN_DRAIN(GPIOA, 10);
	GPIO_config_output(GPIOB, 3);
	GPIO_OPEN_DRAIN(GPIOB, 3);
	GPIO_config_output(GPIOB, 5);
	GPIO_OPEN_DRAIN(GPIOB, 5);
	GPIO_config_output(GPIOB, 4);
	GPIO_OPEN_DRAIN(GPIOB, 4);

	// rows
	// PB10, PA8, PA9, PC7
	GPIO_config_input(GPIOB, 10);
	GPIO_PULL_UP(GPIOB, 10);
	GPIO_config_input(GPIOA, 8);
	GPIO_PULL_UP(GPIOA, 8);
	GPIO_config_input(GPIOA, 9);
	GPIO_PULL_UP(GPIOA, 9);
	GPIO_config_input(GPIOC, 7);
	GPIO_PULL_UP(GPIOC, 7);

	activate_col(-1);
}

char keys[4][4] = { {'1', '2', '3', 'A'},
					{'4', '5', '6', 'B'},
					{'7', '8', '9', 'C'},
					{'*', '0', '#', 'D'}
};


char __keypad_read(void)
{
	int col;
	int r0, r1, r2, r3;
	for (col = 0; col < 4;col++) {
		activate_col(col);
		delay_ms(1);
		r0 = GPIO_read(GPIOB, 10);
		r1 = GPIO_read(GPIOA, 8);
		r2 = GPIO_read(GPIOA, 9);
		r3 = GPIO_read(GPIOC, 7);
		//printf("%d: %d %d %d %d\n", col, r0, r1, r2, r3);
		if (r0 == 0) {
			return keys[col][0];
		}
		if (r1 == 0) {
			return keys[col][1];
		}
		if (r2 == 0) {
			return keys[col][2];
		}
		if (r3 == 0) {
			return keys[col][3];
		}
	}
	//printf("\n");
	return 255;
}

char keypad_read(void)
{
	char c;
	do {
		c = __keypad_read();
	} while (c != 255); // attesa rilascio tasto

	do {
		c = __keypad_read();
	} while (c == 255); // attesa tasto premuto
	return c;
}

int main(void)
{
	ClockConfig();
	CONSOLE_init();
	printf("Starting\n");
	keypad_init();
	for (;;) {
		char c = keypad_read();
		printf("Key = %c\n",c);
	}
}
