#include "stm32f1xx.h"
void delay(uint32_t count){
    while(count--);
}
int main(){
    RCC->APB2ENR |= (1 << 4);
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (2 << 20);
    
    while(1){
        GPIOC->ODR &= ~(1 << 13);
        delay(200000);
        GPIOC->ODR |= (1 << 13);
        delay(200000); 
    }
}
