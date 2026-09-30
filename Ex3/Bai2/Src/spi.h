#ifndef SPI_H
#define SPI_H

#include "stm32f1xx.h"
#include <stdint.h>

void Spi_Init(void);
uint8_t Spi_TransferByte(uint8_t data);

#endif