#include "stm32f1xx.h"

static void GPIO_Init(void)
{
    RCC->APB2ENR |= (1 << 2);

    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |= (0x2 << 0);

    GPIOA->CRL &= ~(0xF << 4);
    GPIOA->CRL |= (0x2 << 4);

    GPIOA->CRL &= ~(0xF << 8);
    GPIOA->CRL |= (0x2 << 8);
}

static void SysTick_Init(void)
{
    SysTick->LOAD = 72000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 7;
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
    SysTick_Init();

    while (1) {
    }
}
