#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"
void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL &= ~((0xF << 0) | (0xF << 4) | (0xF << 8));
    GPIOA->CRL |= ((0x2 << 0) | (0x2 << 4) | (0x2 << 8));
    GPIOA->BRR = (1 << 0) | (1 << 1) | (1 << 2);
}
typedef struct{
    GPIO_TypeDef *port;
    uint16_t pin;
    float f;
} Led_ts;
void LedToggleTask(void *pvParameters){
   Led_ts *ts = (Led_ts *)pvParameters;
   float f = ts->f;
   uint16_t T = (uint16_t)(1000/f);
   while(1){
        ts->port->ODR ^= ts->pin;
        vTaskDelay(pdMS_TO_TICKS(T/2));
   } 
}
static Led_ts led1 = {GPIOA, (1 << 0), 0.1};
static Led_ts led2 = {GPIOA, (1 << 1), 1.0};
static Led_ts led3 = {GPIOA, (1 << 2), 10.0};
int main(){
    GPIO_Init();
    xTaskCreate(LedToggleTask, "LED1", 128, &led1, 1, NULL);
    xTaskCreate(LedToggleTask, "LED2", 128, &led2, 1, NULL);
    xTaskCreate(LedToggleTask, "LED3", 128, &led3, 1, NULL);
    vTaskStartScheduler();
}