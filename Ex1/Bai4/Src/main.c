#include "stm32f1xx.h"

void delay(void)
{
    for (int i = 0; i < 50000; i++);
}

int main(void)
{
    RCC->APB2ENR |= (1 << 4);
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |=  (0x2 << 20);

    GPIOC->CRH &= ~(0xF << 24);
    GPIOC->CRH |=  (0x8 << 24);

    GPIOC->ODR |= (1 << 14);

    GPIOC->ODR |= (1 << 13);

    uint8_t button_pressed = 0;

    while (1)
    {
        if (!(GPIOC->IDR & (1 << 14)))
        {
            button_pressed = 1;
        }
        if ((GPIOC->IDR & (1 << 14)) && button_pressed)
        {
            GPIOC->ODR ^= (1 << 13);
            button_pressed = 0;
            delay();
        }
    }
}