#include "timer.h"
#include "stm32f1xx.h"

void TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Timer clock = 72 MHz
    // 72 MHz / 72 = 1 MHz
    TIM2->PSC = 71;

    // 1 MHz / 1000 = 1 kHz
    TIM2->ARR = 999;

    // Duty cycle
    TIM2->CCR1 = 100;   // 10%
    TIM2->CCR2 = 300;   // 30%
    TIM2->CCR3 = 500;   // 50%
    TIM2->CCR4 = 700;   // 70%

    // CH1 - PWM Mode 1
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 |
                   TIM_CCMR1_OC1PE;

    // CH2 - PWM Mode 1
    TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
    TIM2->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 |
                   TIM_CCMR1_OC2PE;

    // CH3 - PWM Mode 1
    TIM2->CCMR2 &= ~TIM_CCMR2_OC3M;
    TIM2->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 |
                   TIM_CCMR2_OC3PE;

    // CH4 - PWM Mode 1
    TIM2->CCMR2 &= ~TIM_CCMR2_OC4M;
    TIM2->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 |
                   TIM_CCMR2_OC4PE;

    // Enable CH1, CH2, CH3, CH4
    TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E |
                  TIM_CCER_CC3E | TIM_CCER_CC4E;

    // Auto reload preload
    TIM2->CR1 |= TIM_CR1_ARPE;

    // Update event
    TIM2->EGR = TIM_EGR_UG;

    // Start TIM2
    TIM2->CR1 |= TIM_CR1_CEN;
}
