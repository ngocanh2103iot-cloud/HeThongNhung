#include "stm32f1xx.h"
void delay(uint32_t count){
    while(count--);
}
int main(){
    RCC->APB2ENR |= (1 << 2);
    GPIOA->CRL = 0x22222222;
    
    while(1){
         for (int i = 0; i < 8; i++)
        {
            GPIOA->ODR &= ~0xFF;
            GPIOA->ODR |= (1 << i);
            delay(500000);
        }
        for (int i = 6; i >= 1; i--)
        {
            GPIOA->ODR &= ~0xFF;
            GPIOA->ODR |= (1 << i);
            delay(500000);
        }
}
}