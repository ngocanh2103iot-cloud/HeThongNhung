#include "stm32f1xx.h" 
#include "dma_uart.h" 

/* Co dung chung voi ngat: 1 la ranh, 0 la dang truyen. */
volatile uint8_t dma_done = 0; 
/* Cau hinh DMA1 Channel4 gui bo nho sang USART1 va bat ngat. */
void DMA_UART_Init(){ 
    RCC->AHBENR |= 1; 

    /* Chon DR cua USART1 lam dich; truyen tu bo nho, tu tang dia chi va bat ngat xong. */
    DMA1_Channel4->CPAR = (uint32_t)&USART1->DR; 
    DMA1_Channel4->CCR = 0; 
    DMA1_Channel4->CCR |= (1 << 4); 
    DMA1_Channel4->CCR |= (1 << 7); 
    DMA1_Channel4->CCR |= (1 << 1);  
    
    /* Cho phep USART1 yeu cau DMA va mo ngat Channel4 trong NVIC. */
    USART1->CR3 |= (1 << 7);    
    NVIC_EnableIRQ(DMA1_Channel4_IRQn); 
    dma_done=1; 
}
/* Neu DMA ranh, nap bo dem va gui len - 1 byte. */
uint8_t DMA_UART_SendData(uint8_t *data, uint8_t len){ 
    /* Neu lan gui truoc chua xong thi tra 0, khong thay doi bo dem DMA. */
    if(!dma_done) return 0; 
    /* Xoa co cu, nap dia chi nguon va so byte cho lan truyen moi. */
    DMA1->IFCR = (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15); 
    DMA1_Channel4->CMAR = (uint32_t)data; 
    DMA1_Channel4->CNDTR = len - 1; 
    /* Danh dau dang ban truoc khi bat kenh DMA; tra 1 khi da chap nhan gui. */
    dma_done = 0; 
    DMA1_Channel4->CCR |= (1 << 0); 
    return 1; 
}
/* Khi truyen xong, xoa co ngat, tat kenh va bao DMA ranh. */
void DMA1_Channel4_IRQHandler(void) 
{
    /* Chi xu ly co truyen xong: xoa co, tat kenh va cho phep lan gui tiep theo. */
    if (DMA1->ISR & (1 << 13)) 
    {
        DMA1->IFCR = (1 << 13); 
        DMA1_Channel4->CCR &= ~(1 << 0); 
        dma_done = 1; 
    }
}
