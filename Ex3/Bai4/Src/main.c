#include "stm32f1xx.h"
#include "dma_adc.h"
#include "timer.h"
#include "uart.h"

int main(void)
{
    UART_Init();
    ADC_Init();
    DMA_ADC_Init();
    TIM3_Init();

    while (1)
    {
        ADC_Send_Ready_Data();
    }
}
