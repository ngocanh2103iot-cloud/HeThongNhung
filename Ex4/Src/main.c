#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "task.h"


static void GPIO_Init(void)
{
    /* Enable GPIOA clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /*
     * PA0, PA1, PA2 = Output Push-Pull, 2 MHz
     *
     * Moi pin dùng 4 bit trong CRL:
     * PA0 -> bits 3:0
     * PA1 -> bits 7:4
     * PA2 -> bits 11:8
     */
    GPIOA->CRL &= ~((0xFUL << 0) |
                    (0xFUL << 4) |
                    (0xFUL << 8));

    GPIOA->CRL |=  ((0x2UL << 0) |
                    (0x2UL << 4) |
                    (0x2UL << 8));

    /* OFF LED */
    GPIOA->BSRR = (1UL << 0) |
                  (1UL << 1) |
                  (1UL << 2);
}


static void LED_A0_Task(void *argument)
{
    (void)argument;

    while (1)
    {
        GPIOA->ODR ^= (1UL << 0);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


static void LED_A1_Task(void *argument)
{
    (void)argument;

    while (1)
    {
        GPIOA->ODR ^= (1UL << 1);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


static void LED_A2_Task(void *argument)
{
    (void)argument;

    while (1)
    {
        GPIOA->ODR ^= (1UL << 2);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


int main(void)
{
    GPIO_Init();

    xTaskCreate(
        LED_A0_Task,
        "LED_A0",
        128,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        LED_A1_Task,
        "LED_A1",
        128,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        LED_A2_Task,
        "LED_A2",
        128,
        NULL,
        1,
        NULL
    );

    vTaskStartScheduler();

    while (1)
    {
    }
}