#include "pwm_init.h"
#include "stm32f1xx.h"

int led_on = 0;
int pwm_percent = 50;

void Pwm_Init(void){
    RCC->APB2ENR |= (1 << 2);
    RCC->APB1ENR |= (1 << 0);       // TIM2

    TIM2->CR1 = 0;
    TIM2->CCER = 0;
    TIM2->PSC = 71;                // TIM2 clock 72 MHz -> 1 MHz
    TIM2->ARR = 999;               // PWM 1 kHz, 1000 counts
    TIM2->CCR1 = 0;                // Khoi dong: LED tat
    TIM2->CCMR1 = (6 << 4) | (1 << 3); // PWM mode 1, preload CCR1
    TIM2->CR1 = (1 << 7);          // Preload ARR
    TIM2->EGR = (1 << 0);          // Nap PSC, ARR, CCR1
    TIM2->SR = 0;
    TIM2->CCER = (1 << 0);         // CH1 active high

    GPIOA->CRL &= ~0xF;
    GPIOA->CRL |= 0xA;             // PA0: AF push-pull, 2 MHz
    TIM2->CR1 |= (1 << 0);
}

void Led_Update(void){
    if(led_on) TIM2->CCR1 = pwm_percent * 10;
    else TIM2->CCR1 = 0;
}
