#include "spi.h" 

/* Khoi tao SPI1 master, mode 0, clock PCLK2/16. */
void SPI_Init(){ 
    /* Cap clock GPIOA va SPI1 de cau hinh chan va bo truyen. */
    RCC->APB2ENR |= (1 << 2); 
    RCC->APB2ENR |= (1 << 12); 

    /* PA4: CS; PA5, PA7: SCK, MOSI; PA6: MISO. */
    GPIOA->CRL &= ~((0xF << 16) | (0xF << 20 ) | (0xF << 24) | (0xF << 28)); 
    GPIOA->CRL |= (0xB << 20) | (0xB << 28); 
    GPIOA->CRL |= (0x4 << 24); 
    GPIOA->CRL |= (0x3 << 16); 

    GPIOA->BSRR = (1 << 4); 

    /* Cau hinh master, NSS phan mem, mode 0 va bo chia 16; sau do bat SPI. */
    SPI1->CR1 = 0; 
    SPI1->CR1 |= (1 << 9); 
    SPI1->CR1 |= (1 << 8); 
    SPI1->CR1 |= (0x3 << 3); 
    SPI1->CR1 |= (1 << 2); 
    SPI1->CR1 |= (1 << 6); 
}
/* Gui mot byte va cho nhan byte tuong ung. */
uint8_t SPI_TransferByte(uint8_t data){ 
    /* Cho TXE roi ghi byte can gui vao DR. */
    while(!(SPI1->SR & (1 << 1))); 
    SPI1->DR = data; 
    /* Cho RXNE va doc DR de lay byte nhan dong thoi voi byte vua gui. */
    while(!(SPI1->SR & 1)); 
    return (SPI1->DR); 
}
