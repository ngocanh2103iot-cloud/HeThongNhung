#ifndef DMA_ADC_H
#define DMA_ADC_H

#include <stdint.h>

extern uint16_t adc_buffer[100];

void ADC_Init(void);
void DMA_ADC_Init(void);
void ADC_Send_Ready_Data(void);

#endif
