#include "timer.h"
void TIM3_Init(void)
{
    /*  Bật clock TIM3 */
    RCC->APB1ENR |= (1 << 1);

    /*  Timer = 100 Hz */
    TIM3->PSC = 7199;
    TIM3->ARR = 99;

    /*  Nạp PSC và ARR */
    TIM3->EGR |= (1 << 0);
    
    /*  TRGO lấy từ Update Event
       MMS[2:0] = 010, bit 6:4 */
    TIM3->CR2 &= ~(7 << 4);
    TIM3->CR2 |=  (2 << 4);

    /*  Start Timer */
    TIM3->CR1 |= (1 << 0);
}
