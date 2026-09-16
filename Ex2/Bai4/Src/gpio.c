#include "gpio.h"
#include "stm32f1xx.h"

void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /*
     * PA0 -> TIM2_CH1
     * PA1 -> TIM2_CH2
     * PA2 -> TIM2_CH3
     * PA3 -> TIM2_CH4
     *
     * Alternate Function Push-Pull
     * 50 MHz
     *
     * MODE = 11
     * CNF  = 10
     * => 1011 = 0xB
     */

    GPIOA->CRL = (GPIOA->CRL &
                  ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                    GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
                    GPIO_CRL_MODE2 | GPIO_CRL_CNF2 |
                    GPIO_CRL_MODE3 | GPIO_CRL_CNF3)) |
                 GPIO_CRL_MODE0 | GPIO_CRL_CNF0_1 |
                 GPIO_CRL_MODE1 | GPIO_CRL_CNF1_1 |
                 GPIO_CRL_MODE2 | GPIO_CRL_CNF2_1 |
                 GPIO_CRL_MODE3 | GPIO_CRL_CNF3_1;
}
