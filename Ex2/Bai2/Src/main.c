#include "stm32f1xx.h"

static void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                    GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
                    GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1;
}

void SysTick_Handler(void)
{
    static uint16_t led_01hz_ticks;
    static uint16_t led_1hz_ticks;
    static uint16_t led_10hz_ticks;

    if (++led_01hz_ticks == 5000U) {
        led_01hz_ticks = 0U;
        GPIOA->ODR ^= GPIO_ODR_ODR0;
    }

    if (++led_1hz_ticks == 500U) {
        led_1hz_ticks = 0U;
        GPIOA->ODR ^= GPIO_ODR_ODR1;
    }

    if (++led_10hz_ticks == 50U) {
        led_10hz_ticks = 0U;
        GPIOA->ODR ^= GPIO_ODR_ODR2;
    }
}

int main(void)
{
    GPIO_Init();
    SysTick_Config(SystemCoreClock / 1000U);

    while (1) {
        __WFI();
    }
}
