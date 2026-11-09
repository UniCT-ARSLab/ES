/*
 * imu.c
 *
 *  Created on: Nov 26, 2018
 *      Author: corrado
 */

#include "stm32_unict_lib.h"
#include <stdio.h>

#define QUEUE_SIZE 40
typedef struct {
	int size, head, tail;
	char data[QUEUE_SIZE];
} t_queue;

enum {
	WAIT_KEY_PRESS,
	WAIT_KEY_RELEASE
};

struct s_key_state {
	t_queue key_queue;
	int halt_scan;
	int current_state;
	int current_column;
	char current_key;
} key_state;

void init_queue(void)
{
	key_state.key_queue.size = 0;
	key_state.key_queue.head = 0;
	key_state.key_queue.tail = 0;
}

void enqueue(char c)
{
	if (key_state.key_queue.size < QUEUE_SIZE) {
		key_state.key_queue.data[key_state.key_queue.head] = c;
		key_state.key_queue.head = (key_state.key_queue.head + 1) % QUEUE_SIZE;
		key_state.key_queue.size ++;
	}
}

int dequeue(char * c)
{
	key_state.halt_scan = 1;
	if (key_state.key_queue.size > 0) {
		*c = key_state.key_queue.data[key_state.key_queue.tail];
		key_state.key_queue.tail = (key_state.key_queue.tail + 1) % QUEUE_SIZE;
		key_state.key_queue.size --;
		key_state.halt_scan = 0;
		return 1;
	}
	else {
		key_state.halt_scan = 0;
		return 0;
	}
}

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

	init_queue();

	key_state.current_column = 0;
	key_state.current_key = 255;
	key_state.current_state = WAIT_KEY_PRESS;
	key_state.halt_scan = 0;
	activate_col(0);

	TIM_init(TIM4);
	TIM_config_timebase(TIM4, 84, 1000);
	TIM_enable_irq(TIM4, IRQ_UPDATE);
	TIM_set(TIM4,0);
	TIM_on(TIM4);

}

char keys[4][4] = { {'1', '2', '3', 'A'},
					{'4', '5', '6', 'B'},
					{'7', '8', '9', 'C'},
					{'*', '0', '#', 'D'}
};

void TIM4_IRQHandler(void)
{
	if (TIM_update_check(TIM4)) {
		if (!key_state.halt_scan) {
			int r0, r1, r2, r3;
			// leggi la colonna corrente
			r0 = GPIO_read(GPIOB, 10);
			r1 = GPIO_read(GPIOA, 8);
			r2 = GPIO_read(GPIOA, 9);
			r3 = GPIO_read(GPIOC, 7);
			if (r0 == 0) {
				key_state.current_key = keys[key_state.current_column][0];
			}
			if (r1 == 0) {
				key_state.current_key = keys[key_state.current_column][1];
			}
			if (r2 == 0) {
				key_state.current_key = keys[key_state.current_column][2];
			}
			if (r3 == 0) {
				key_state.current_key = keys[key_state.current_column][3];
			}
			// attiva la colonna successiva
			++key_state.current_column;
			if (key_state.current_column == 4) {
				// scansione terminata
				if (key_state.current_key != 255) {
					// tasto premuto
					if (key_state.current_state == WAIT_KEY_PRESS) {
						enqueue(key_state.current_key);
						key_state.current_state = WAIT_KEY_RELEASE;
					}
				}
				else {
					// tasto rilasciato
					if (key_state.current_state == WAIT_KEY_RELEASE)
						key_state.current_state = WAIT_KEY_PRESS;
				}
				key_state.current_column = 0;
				key_state.current_key = 255;
			}
			activate_col(key_state.current_column);
		}
		TIM_update_clear(TIM4);
	}
}

char keypad_read(void)
{
	char c;
	while (!dequeue(&c)) ;
	return c;
}

int main(void)
{
	ClockConfig();
	CONSOLE_init();
	printf("Starting\n");
	keypad_init();
	delay_ms(10000);
	printf("Reading\n");
	for (;;) {
		char c = keypad_read();
		printf("Key = %c\n",c);
		//printf("Queue size = %d\n", key_queue.size);
	}
}
