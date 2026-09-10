#include "stm32f1xx.h"

static void gpio_init(void)
{
    RCC->APB2ENR |= (1 << 2);

    GPIOA->CRL = 0x88888888;
    GPIOA->ODR |= 0x00FF;

    GPIOA->CRH = 0x22222222;
}

int main(void)
{
    uint32_t input;
    uint32_t output;

    gpio_init();

    while (1)
    {
        input = GPIOA->IDR & 0xFF;

        output = (~input) & 0xFF;

        GPIOA->ODR = (GPIOA->ODR & 0x00FF) | (output << 8);
    }
}