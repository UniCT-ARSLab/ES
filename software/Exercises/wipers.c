

#include "stm32_unict_lib.h"

typedef enum {
    st_OFF,
    st_RAIN,
    st_TIMED_ON,
    st_TIMED_OFF,
    st_CONT,
    st_TO_OFF
} t_state;

#define TS1     200   // 2 sec
#define TS2     100    // 1 sec

char *wipers_display[] = {
    "-   ",
    " -  ",
    "  - ",
    "   -",
    "   -",
    "  - ",
    " -  ",
    "-   "};

t_state state = st_OFF;
volatile int wipers_pos = 0;
int wipers_duration, wipers_step_duration;
int wipers_step_counter = 0;

int wait_time = 100;
int wait_time_counter = 0;

int rain_speed, drop_time;
int drop_time_counter;
int drops;

void setup(void)
{
    ClockConfig();

    DISPLAY_init();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOB, 9); // configure pin PB9 as output
    GPIO_config_output(GPIOC, 2); // configure pin PC2 as output
    GPIO_config_output(GPIOC, 3); // configure pin PC3 as output

    GPIO_config_input(GPIOB, 10); // configure pin PB10 as input
    GPIO_config_input(GPIOB, 4); // configure pin PB4 as input
    GPIO_config_input(GPIOB, 5); // configure pin PB5 as input
    GPIO_config_input(GPIOB, 6); // configure pin PB6 as input
   
    GPIO_config_EXTI(GPIOB, EXTI10); // attach PB10 to EXTI10
    GPIO_config_EXTI(GPIOB, EXTI4); // attach PB4 to EXTI4
    GPIO_config_EXTI(GPIOB, EXTI5); // attach PB5 to EXTI5
    GPIO_config_EXTI(GPIOB, EXTI6); // attach PB6 to EXTI6

    EXTI_enable(EXTI10, FALLING_EDGE);
    EXTI_enable(EXTI4, FALLING_EDGE);
    EXTI_enable(EXTI5, FALLING_EDGE);
    EXTI_enable(EXTI6, FALLING_EDGE);

	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 42000, 20);
    // increment frequency = 84e6 / 42e3 = 2000 Hz
    // autoreload = 20 --> update every 1/100 sec
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);

    ADC_init(ADC1, ADC_RES_10, ADC_ALIGN_RIGHT);
    ADC_channel_config(ADC1, GPIOC, 0, 10); // PC0 su channel 10
    ADC_channel_config(ADC1, GPIOC, 1, 11); // PC1 su channel 11
    ADC_on(ADC1);

}

void TIM2_IRQHandler(void)
{
    if (TIM_update_check(TIM2)) {
        if (state != st_OFF) {

            if (state == st_RAIN) {
                if (rain_speed > 0) {
                    ++drop_time_counter;
                    if (drop_time_counter >= drop_time) {
                        drop_time_counter = 0;
                        GPIO_toggle(GPIOB, 9);
                        ++drops;
                    }
                }
            }

            int do_wipe = 0;

            switch(state) {
                case st_CONT:
                case st_TIMED_ON:
                case st_TO_OFF:
                    do_wipe = 1;
                    break;
                case st_RAIN:
                    if (drops >= 10) {
                        do_wipe = 1;
                    }
                    break;
            }

            if (do_wipe) {

                ++wipers_step_counter;
                if (wipers_step_counter >= wipers_step_duration) {
                    wipers_step_counter = 0;
                    ++wipers_pos;
                    if (wipers_pos == 8) {
                        wipers_pos = 0;
                        switch(state) {
                            case st_RAIN:
                                drops = 0;
                                break;
                            case st_TO_OFF:
                                state = st_OFF;
                                break;
                            case st_TIMED_ON:
                                state = st_TIMED_OFF;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            if (state == st_TIMED_OFF) {
                ++wait_time_counter;
                if (wait_time_counter >= wait_time) {
                    wait_time_counter = 0;
                    state = st_TIMED_ON;
                }
            }
        }
        TIM_update_clear(TIM2);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {
        // pushbutton X
        if (state != st_RAIN) {
            wipers_duration = TS1;
            wipers_step_duration = wipers_duration / 8;
            state = st_RAIN;
        }
        else
            state = st_TO_OFF;
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // pushbutton Y
        if (state != st_TIMED_ON && state != st_TIMED_OFF) {
            wipers_duration = TS1;
            wipers_step_duration = wipers_duration / 8;
            state = st_TIMED_ON;
        }
        else {
            state = st_TO_OFF;
        }
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        // pushbutton Z
        if (state != st_CONT) {
            wipers_duration = TS2;
            wipers_step_duration = wipers_duration / 8;
            state = st_CONT;
        }
        else {
            state = st_TO_OFF;
        }
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        // pushbutton T
        EXTI_clear(EXTI6);
    }
}

void loop(void)
{

    if (state == st_TIMED_ON || state == st_TIMED_OFF) {
        ADC_sample_channel(ADC1, 10);
        ADC_start(ADC1);
        while (!ADC_completed(ADC1)) {}
        int value = ADC_read(ADC1);

        wait_time = (value * 400)/1023 + 100;
    }

    if (state == st_RAIN) {
        ADC_sample_channel(ADC1, 11);
        ADC_start(ADC1);
        while (!ADC_completed(ADC1)) {}
        int value = ADC_read(ADC1);

        rain_speed = (value * 500) / 1023;

        if (rain_speed != 0)
            drop_time = 100 / rain_speed; // rain_speed = [0, 500]

    }

    if (state == st_OFF)
        DISPLAY_puts(0, wipers_display[0]);
    else
        DISPLAY_puts(0, wipers_display[wipers_pos]);

    switch (state)
    {
    case st_OFF:
        GPIO_write(GPIOB, 0, 0);
        GPIO_write(GPIOC, 2, 0);
        GPIO_write(GPIOC, 3, 0);
        break;
    
    case st_RAIN:
        GPIO_write(GPIOB, 0, 1);
        GPIO_write(GPIOC, 2, 0);
        GPIO_write(GPIOC, 3, 0);
        break;

    case st_TIMED_ON:
    case st_TIMED_OFF:
        GPIO_write(GPIOB, 0, 0);
        GPIO_write(GPIOC, 2, 1);
        GPIO_write(GPIOC, 3, 0);
        break;
        
    case st_CONT:
        GPIO_write(GPIOB, 0, 0);
        GPIO_write(GPIOC, 2, 0);
        GPIO_write(GPIOC, 3, 1);
        break;
        
    default:
        break;   
    }
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
