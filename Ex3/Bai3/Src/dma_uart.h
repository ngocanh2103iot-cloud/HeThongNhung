#ifndef DMA_UART_H 
#define DMA_UART_H 

#include <stdint.h> 

/* 1: DMA san sang gui tiep; 0: dang truyen. */
extern volatile uint8_t dma_done; 

/* Dung USART1 TX va DMA1 Channel4; goi sau UART_Init. */
void DMA_UART_Init(void); 
/* Gui len - 1 byte; giu bo dem den khi dma_done = 1. */
uint8_t DMA_UART_SendData(uint8_t *data, uint8_t len); 

#endif 
