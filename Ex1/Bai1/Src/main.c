#include "stm32f1xx.h"

void delay(uint32_t time)
{
    while (time--);
}

int main(void)
{
    RCC->APB2ENR |= (1 << 4);
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x2 << 20);

    while (1)
    {
        GPIOC->ODR &= ~(1 << 13);
        delay(500000);
        GPIOC->ODR |= (1 << 13);
        delay(500000);
    }
}