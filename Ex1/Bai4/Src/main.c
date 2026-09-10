#include "stm32f1xx.h"


int main(){
    RCC->APB2ENR |= (1 << 4);

    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x2 << 20);

    GPIOC->CRH &= ~(0xF << 24);
    GPIOC->CRH |= (0x8 <<24);

    GPIOC->ODR |= (1 << 13);
    GPIOC->ODR |= (1 << 14);
    uint8_t cur;
    uint8_t pre = 1;
    while(1){
        if(!(GPIOC->IDR & (1 << 14))) cur = 0; 
            else cur = 1;
        if(pre == 0 && cur == 1) 
            GPIOC->ODR ^= (1 << 13);
        pre = cur;
    }
}