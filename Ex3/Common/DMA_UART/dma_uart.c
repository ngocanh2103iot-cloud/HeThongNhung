#include "stm32f1xx.h"
#include "dma_uart.h"

volatile uint8_t dma_done = 0;
void Dma_Uart_Init(){
    RCC->AHBENR |= 1;

    DMA1_Channel4->CPAR = &USART1->DR;
    DMA1_Channel4->CCR = 0;
    DMA1_Channel4->CCR |= (1 << 4);
    DMA1_Channel4->CCR |= (1 << 7);
    DMA1_Channel4->CCR |= (1 << 1); 
    
    USART1->CR3 |= (1 << 7);   
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    dma_done=1;
}
uint8_t Dma_Uart_SendData(uint8_t *data, uint8_t len){
    if(!dma_done) return 0;
    DMA1->IFCR = (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15);
    DMA1_Channel4->CMAR = data;
    DMA1_Channel4->CNDTR = len - 1;
    dma_done = 0;
    DMA1_Channel4->CCR |= (1 << 0);
    return 1;
}
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & (1 << 13))
    {
        DMA1->IFCR = (1 << 13);
        DMA1_Channel4->CCR &= ~(1 << 0);
        dma_done = 1;
    }
}

