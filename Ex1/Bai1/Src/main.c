#include "stm32f1xx.h"

int main(void)
{
    /* =========================
       PC13 - LED
       ========================= */

    // Bật clock GPIOC
    RCC->APB2ENR |= (1U << 4);

    // PC13: Output Push-Pull, 2 MHz
    GPIOC->CRH &= ~(0xFU << 20);
    GPIOC->CRH |=  (0x2U << 20);

    // LED OFF ban đầu (PC13 active LOW)
    GPIOC->ODR |= (1U << 13);


    /* =========================
       TIM2
       ========================= */

    // Bật clock TIM2
    RCC->APB1ENR |= (1U << 0);

    /*
        Giả sử TIM2CLK = 8 MHz

        8 MHz / 8000 = 1000 Hz

        => CNT tăng 1 lần mỗi 1 ms
    */
    TIM2->PSC = 8000 - 1;

    /*
        Đếm 1000 lần:

        1000 × 1 ms = 1 giây
    */
    TIM2->ARR = 1000 - 1;

    TIM2->CNT = 0;

    // Nạp PSC vào timer
    TIM2->EGR |= (1U << 0);

    // Xóa cờ Update
    TIM2->SR &= ~(1U << 0);

    // Start TIM2
    TIM2->CR1 |= (1U << 0);


    while (1)
    {
        // Kiểm tra cờ Update Interrupt Flag (UIF)
        if (TIM2->SR & (1U << 0))
        {
            // Xóa cờ
            TIM2->SR &= ~(1U << 0);

            // Đảo trạng thái PC13
            GPIOC->ODR ^= (1U << 13);
        }
    }
}