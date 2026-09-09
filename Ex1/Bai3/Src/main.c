#include "stm32f1xx.h"

static void clock_init(void)
{
    FLASH->ACR &= ~0x7U;
    FLASH->ACR |=  0x2U;
    RCC->CR |= (1U << 16);

    // Wait HSE ready
    while (!(RCC->CR & (1U << 17)))
    {
    }
    RCC->CFGR = 0;
    RCC->CFGR |= (4U << 8);
    RCC->CFGR |= (1U << 16);
    RCC->CFGR |= (7U << 18);
    RCC->CR |= (1U << 24);

    while (!(RCC->CR & (1U << 25)))
    {
    }
    RCC->CFGR &= ~(3U << 0);
    RCC->CFGR |=  (2U << 0);
    while ((RCC->CFGR & (3U << 2)) != (2U << 2))
    {
    }
}

static void systick_init(void)
{
    SysTick->LOAD = 72000U - 1U;
    SysTick->VAL = 0;
    SysTick->CTRL = (1U << 2) | (1U << 0);
}

static void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        while (!(SysTick->CTRL & (1U << 16)))
        {
        }
    }
}

static void gpio_init(void)
{
    RCC->APB2ENR |= (1U << 2);
    RCC->APB2ENR |= (1U << 3);
    GPIOA->CRL = 0x22222222;
    GPIOB->CRH = 0x88888888;
    GPIOB->BSRR = 0x0000FF00;
}

int main(void)
{
    clock_init();
    systick_init();
    gpio_init();

    while (1)
    {
        uint32_t input;
        uint32_t output;
        input = (GPIOB->IDR >> 8) & 0xFF;
        output = input;
        GPIOA->ODR =
            (GPIOA->ODR & 0xFFFFFF00) | output;
    }
}