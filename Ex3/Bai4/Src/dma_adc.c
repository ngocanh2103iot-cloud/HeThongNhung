
#include "stm32f1xx.h"
#include "dma_adc.h"
#include "uart.h"

uint16_t adc_buffer[100];
static volatile uint8_t first_half_ready = 0U;
static volatile uint8_t second_half_ready = 0U;

void ADC_Init(){
    RCC->APB2ENR |= (1 << 2) | (1 << 9);  // Cap xung GPIOA va ADC1
    RCC->CFGR &= ~(0x3 << 14);            // Xoa cau hinh prescaler ADC
    RCC->CFGR |= (0x2 << 14);             // PCLK2 / 6 = 12 MHz

    GPIOA->CRL &= ~(0xF << 0);            // PA0 analog input

    ADC1->SMPR2 &= ~(0x7 << 0);           // Xoa sample time kenh 0
    ADC1->SMPR2 |= (0x7 << 0);            // Sample time 239.5 chu ky
    ADC1->SQR1 &= ~(0xF << 20);           // Mot kenh trong regular sequence
    ADC1->SQR3 &= ~(0x1F << 0);           // Kenh dau tien la ADC channel 0
    ADC1->CR2 &= ~(1 << 1);
    ADC1->CR2 |= (1 << 20);
    ADC1->CR2 &= ~(0x7 << 17);
    ADC1->CR2 |= (0x4 << 17);
    ADC1->CR2 |= (1 << 8);
    ADC1->CR2 |= (1 << 0);
    
    for (volatile uint32_t i = 0; i < 1000; i++);
    ADC1->CR2 |= (1 << 3);
    while (ADC1->CR2 & (1 << 3));
    ADC1->CR2 |= (1 << 2);
    while (ADC1->CR2 & (1 << 2));
}

void ADC_Send_Ready_Data(void)
{
    if (first_half_ready )
    {
        first_half_ready = 0;
        UART_Send_U16_Buffer(&adc_buffer[0], 50U);
    }

    if (second_half_ready)
    {
        second_half_ready = 0;
        UART_Send_U16_Buffer(&adc_buffer[50], 50U);
    }
}

void DMA_ADC_Init(void)
{
    /* 1. Bật clock DMA1 */
    RCC->AHBENR |= (1 << 0);

    /* 2. Tắt DMA channel trước khi cấu hình */
    DMA1_Channel1->CCR &= ~(1 << 0);

    /* 3. Địa chỉ nguồn: ADC1->DR */
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;

    /* 4. Địa chỉ đích: buffer RAM */
    DMA1_Channel1->CMAR = (uint32_t)adc_buffer;

    /* 5. Tổng số dữ liệu cần truyền */
    DMA1_Channel1->CNDTR = 100;

    /* 6. Peripheral -> Memory
       DIR = 0 */
    DMA1_Channel1->CCR &= ~(1 << 4);

    /* 7. Circular mode */
    DMA1_Channel1->CCR |= (1 << 5);

    /* 8. Peripheral address không tăng */
    DMA1_Channel1->CCR &= ~(1 << 6);

    /* 9. Memory address tự tăng */
    DMA1_Channel1->CCR |= (1 << 7);

    /* 10. Peripheral size = 16 bit
       PSIZE = 01 */
    DMA1_Channel1->CCR &= ~(3 << 8);
    DMA1_Channel1->CCR |=  (1 << 8);

    /* 11. Memory size = 16 bit
       MSIZE = 01 */
    DMA1_Channel1->CCR &= ~(3 << 10);
    DMA1_Channel1->CCR |=  (1 << 10);

    /* 12. Half Transfer interrupt */
    DMA1_Channel1->CCR |= (1 << 2);

    /* 13. Transfer Complete interrupt */
    DMA1_Channel1->CCR |= (1 << 1);

    /* 14. Cho phép IRQ DMA1 Channel 1 */
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* 15. Enable DMA */
    DMA1_Channel1->CCR |= (1 << 0);
}
void DMA1_Channel1_IRQHandler(void)
{
    /* Half Transfer: HTIF1 = bit 2 */
    if (DMA1->ISR & (1 << 2))
    {
        /* Xóa cờ HT */
        DMA1->IFCR |= (1 << 2);

        /* buffer[0] -> buffer[49] da an toan */
        first_half_ready = 1;
    }

    /* Transfer Complete: TCIF1 = bit 1 */
    if (DMA1->ISR & (1 << 1))
    {
        /* Xóa cờ TC */
        DMA1->IFCR |= (1 << 1);

        /* buffer[50] -> buffer[99] da an toan */
        second_half_ready = 1;
    }
}
