#include "i2c.h"

void I2c_Init()
{
    RCC->APB2ENR |= (1 << 3); // Cap clock GPIOB
    RCC->APB1ENR |= (1 << 21); // Cap clock I2C1

    /* PB6, PB7: chuc nang thay the cuc mang ho, 50 MHz. */
    GPIOB->CRL &= ~(0xF << 24);
    GPIOB->CRL |= (0xF << 24);
    GPIOB->CRL &= ~(0xF << 28);
    GPIOB->CRL |= (0xF << 28);

    I2C1->CR1  = 0;
    /* PCLK1 = 36 MHz; SCL = 36 MHz / (2 * 180) = 100 kHz. */
    I2C1->CR2 = 0x0024;
    I2C1->CCR = 0x00B4;
    I2C1->TRISE = 37;
    I2C1->CR1 = 1;
}
/* Ghi mot byte vao thanh ghi cua thiet bi. */
void I2C_WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t data){
    while (I2C1->SR2 & (1 << 1)); // Cho bus ranh
    I2C1->CR1 |= (1 << 8); // Phat START
    while (!(I2C1->SR1 & 1)); // Doi bit SB
    I2C1->DR = (dev_addr << 1); 
    while (!(I2C1->SR1 & (1 << 1)));// Cho co ADDR
    (void)I2C1->SR1;
    (void)I2C1->SR2;// Xoa co ADDR
    while(!(I2C1->SR1 & (1 << 7)));
    I2C1->DR = reg_addr;
    while(!(I2C1->SR1 & (1 << 7))); //Doi thanh ghi DR trong
    I2C1->DR = data;
    while(!(I2C1->SR1 & (1 << 2)));
    I2C1->CR1 |= (1 << 9); // Phat STOP
}
/* Gui dia chi thanh ghi, START lap lai roi doc mot byte. */
uint8_t I2C_ReadRegister(uint8_t dev_addr, uint8_t reg_addr){
    while(I2C1->SR2 & (1 << 1));
    I2C1->CR1 |= (1 << 8);
    while(!(I2C1->SR1 & 1));
    I2C1->DR = (dev_addr << 1);
    while(!(I2C1->SR1 & (1 << 1)));
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    while(!(I2C1->SR1 & (1 << 7)));
    I2C1->DR = reg_addr;
    while(!(I2C1->SR1 & (1 << 2)));

    I2C1->CR1 |= (1 << 8);
    while(!(I2C1->SR1 & 1));
    I2C1->DR = (dev_addr << 1) | 1;
    while(!(I2C1->SR1 & (1 << 1)));
    /* Tat ACK de ket thuc nhan. */
    I2C1->CR1 &= ~(1 << 10);
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    I2C1->CR1 |= (1 << 9);
    while(!(I2C1->SR1 & (1 << 6)));
    uint8_t data = I2C1->DR;
    return data;
}
/* Doc lien tiep; phan cuoi xu ly 3 byte con lai. */
void I2C_ReadRegisters(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len){
    while(I2C1->SR2 & (1 << 1));
    I2C1->CR1 |= (1 << 8);
    while(!(I2C1->SR1 & 1));
    I2C1->DR = (dev_addr << 1);
    while(!(I2C1->SR1 & (1 << 1)));
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    while(!(I2C1->SR1 & (1 << 7)));
    I2C1->DR = reg_addr;
    while(!(I2C1->SR1 & (1 << 2)));

    I2C1->CR1 |= (1 << 10);
    I2C1->CR1 &= ~(1 << 11);
    I2C1->CR1 |= (1 << 8);
    while(!(I2C1->SR1 & 1));
    I2C1->DR = (dev_addr << 1) | 1;
    while(!(I2C1->SR1 & (1 << 1)));
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    uint8_t i = 0;
    while(len > 3){
        while(!(I2C1->SR1 & (1 << 6)));
        data[i] = I2C1->DR;
        i++; len--;
    }
    while(!(I2C1->SR1 & (1 << 2)));
    /* Tat ACK de ket thuc nhan. */
    I2C1->CR1 &= ~(1 << 10);
    data[i++] = I2C1->DR;
    I2C1->CR1 |= (1 << 9);
    data[i++] = I2C1->DR;
    while (!(I2C1->SR1 & (1 << 6)));
    data[i] = I2C1->DR;
    
    if (len <= 2)
    return;
}
