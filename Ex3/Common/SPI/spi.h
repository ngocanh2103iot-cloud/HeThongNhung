#ifndef SPI_H 
#define SPI_H 

#include "stm32f1xx.h" 
#include <stdint.h> 

/* Khoi tao SPI1 va truyen nhan mot byte bang polling. */
void SPI_Init(void); 
uint8_t SPI_TransferByte(uint8_t data); 

#endif 
