#include "spi.h"

void Spi_Init(){
    RCC->APB2ENR |= (1 << 2);
    RCC->APB2ENR |= (1 << 12);

    GPIOA->CRL &= ~((0xF << 16) | (0xF << 20 ) | (0xF << 24) | (0xF << 28));
    GPIOA->CRL |= (0xB << 20) | (0xB << 28);
    GPIOA->CRL |= (0x4 << 24);
    GPIOA->CRL |= (0x3 << 16);

    GPIOA->BSRR = (1 << 4);

    SPI1->CR1 = 0;
    SPI1->CR1 |= (1 << 9);
    SPI1->CR1 |= (1 << 8);
    SPI1->CR1 |= (0x3 << 3);
    SPI1->CR1 |= (1 << 2);
    SPI1->CR1 |= (1 << 6);
}
uint8_t Spi_TransferByte(uint8_t data){
    while(!(SPI1->SR & (1 << 1)));
    SPI1->DR = data;
    while(!(SPI1->SR & 1));
    return (SPI1->DR);
}
