

#include "stm32_unict_lib.h"

typedef enum {
    st_SEL,
    st_EROG,
    st_FINE_EROG
} t_state;

t_state state = st_SEL;
int importo = 0;
float importo_erogato = 0;
int flash_counter = 0;

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    DISPLAY_init();
    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 2); // configure pin PC2 as output

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
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {
        // pushbutton X
        if (state == st_SEL)
            importo += 20;
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // pushbutton Y
        if (state == st_SEL)
            importo += 5;
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        // pushbutton Z
        if (state == st_SEL)
            importo = 0;
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        // pushbutton T
        switch(state) {
            case st_SEL:
                importo_erogato = 0;
                flash_counter = 0;
                state = st_EROG;
                break;
            case st_FINE_EROG:
                importo = 0;
                state = st_SEL;
                GPIO_write(GPIOB,0,0);
                break;
        }
        EXTI_clear(EXTI6);
    }
}

void loop(void)
{
    char s[5];
    switch (state) {
        case st_SEL:
            sprintf(s,"%4d", importo);
            DISPLAY_puts(0,s);
            DISPLAY_dp(1, 0);
            break;
        case st_EROG:
            delay_ms(10);

            flash_counter++;
            if (flash_counter == 50) {
                GPIO_toggle(GPIOC,2);
                flash_counter = 0;
            }

            importo_erogato += 0.005 * 1.5;

            int temp = importo_erogato * 100;
            sprintf(s,"%4d", temp);
            DISPLAY_puts(0,s);
            DISPLAY_dp(1, 1);

            if (importo_erogato >= importo) {
                GPIO_write(GPIOC,2,0);
                GPIO_write(GPIOB,0,1);
                state = st_FINE_EROG;
            }

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
