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


volatile int current_column;
volatile char current_key;

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

	current_column = 0;
	current_key = 255;
	activate_col(0);

	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 84, 10000);
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_set(TIM2,0);
	TIM_on(TIM2);

}

char keys[4][4] = { {'1', '2', '3', 'A'},
					{'4', '5', '6', 'B'},
					{'7', '8', '9', 'C'},
					{'*', '0', '#', 'D'}
};

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
		int r0, r1, r2, r3;
		if (current_column < 4) {
			// leggi la colonna corrente
			r0 = GPIO_read(GPIOB, 10);
			r1 = GPIO_read(GPIOA, 8);
			r2 = GPIO_read(GPIOA, 9);
			r3 = GPIO_read(GPIOC, 7);
			if (r0 == 0) {
				current_key = keys[current_column][0];
			}
			if (r1 == 0) {
				current_key = keys[current_column][1];
			}
			if (r2 == 0) {
				current_key = keys[current_column][2];
			}
			if (r3 == 0) {
				current_key = keys[current_column][3];
			}
			// attiva la colonna successiva
			++current_column;
			if (current_column < 4)
				activate_col(current_column);
			else
				activate_col(-1);
		}
		TIM_update_clear(TIM2);
	}
}


char __keypad_read(void)
{
	current_key = 255;
	activate_col(0);
	current_column = 0;
	while (current_column < 4) ;
		//printf("%d %d\n", current_column, current_key);
	return current_key;
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
