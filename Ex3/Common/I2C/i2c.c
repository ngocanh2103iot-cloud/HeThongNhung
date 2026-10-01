#include "i2c.h" 

/* Cap clock va cau hinh I2C1 o toc do 100 kHz. */
void I2C_Init() 
{
    /* Cap clock GPIOB va I2C1 truoc khi truy cap thanh ghi. */
    RCC->APB2ENR |= (1 << 3); 
    RCC->APB1ENR |= (1 << 21); 

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
    /* Cho bus ranh, phat START va doi SB truoc khi gui dia chi. */
    while (I2C1->SR2 & (1 << 1)); 
    I2C1->CR1 |= (1 << 8); 
    while (!(I2C1->SR1 & 1)); 
    /* Gui dia chi thiet bi voi bit ghi; doi ADDR, doc SR1 roi SR2 de xoa co. */
    I2C1->DR = (dev_addr << 1);  
    while (!(I2C1->SR1 & (1 << 1)));
    (void)I2C1->SR1; 
    (void)I2C1->SR2;
    while(!(I2C1->SR1 & (1 << 7))); 
    /* Gui dia chi thanh ghi ben trong thiet bi de chon noi doc/ghi. */
    I2C1->DR = reg_addr; 
    while(!(I2C1->SR1 & (1 << 7))); 
    /* Gui byte du lieu; doi BTF bao truyen xong roi phat STOP. */
    I2C1->DR = data; 
    while(!(I2C1->SR1 & (1 << 2))); 
    I2C1->CR1 |= (1 << 9); 
}
/* Gui dia chi thanh ghi, START lap lai roi doc mot byte. */
uint8_t I2C_ReadRegister(uint8_t dev_addr, uint8_t reg_addr){ 
    /* Cho bus ranh, phat START va doi SB truoc khi gui dia chi. */
    while(I2C1->SR2 & (1 << 1)); 
    I2C1->CR1 |= (1 << 8); 
    while(!(I2C1->SR1 & 1)); 
    /* Gui dia chi thiet bi voi bit ghi; doi ADDR, doc SR1 roi SR2 de xoa co. */
    I2C1->DR = (dev_addr << 1); 
    while(!(I2C1->SR1 & (1 << 1))); 
    (void)I2C1->SR1; 
    (void)I2C1->SR2; 
    while(!(I2C1->SR1 & (1 << 7))); 
    /* Gui dia chi thanh ghi ben trong thiet bi de chon noi doc/ghi. */
    I2C1->DR = reg_addr; 
    while(!(I2C1->SR1 & (1 << 2))); 

    I2C1->CR1 |= (1 << 8); 
    while(!(I2C1->SR1 & 1)); 
    /* Gui lai dia chi thiet bi voi bit doc; doi ADDR truoc khi nhan. */
    I2C1->DR = (dev_addr << 1) | 1; 
    while(!(I2C1->SR1 & (1 << 1))); 
    /* Tat ACK truoc khi xoa ADDR, yeu cau STOP va cho RXNE de doc byte cuoi. */
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
    /* Cho bus ranh, phat START va doi SB truoc khi gui dia chi. */
    while(I2C1->SR2 & (1 << 1)); 
    I2C1->CR1 |= (1 << 8); 
    while(!(I2C1->SR1 & 1)); 
    /* Gui dia chi thiet bi voi bit ghi; doi ADDR, doc SR1 roi SR2 de xoa co. */
    I2C1->DR = (dev_addr << 1); 
    while(!(I2C1->SR1 & (1 << 1))); 
    (void)I2C1->SR1; 
    (void)I2C1->SR2; 
    while(!(I2C1->SR1 & (1 << 7))); 
    /* Gui dia chi thanh ghi ben trong thiet bi de chon noi doc/ghi. */
    I2C1->DR = reg_addr; 
    while(!(I2C1->SR1 & (1 << 2))); 

    /* Bat ACK, xoa POS va phat START lap lai de chuyen sang che do doc. */
    I2C1->CR1 |= (1 << 10); 
    I2C1->CR1 &= ~(1 << 11); 
    I2C1->CR1 |= (1 << 8); 
    while(!(I2C1->SR1 & 1)); 
    /* Gui lai dia chi thiet bi voi bit doc; doi ADDR truoc khi nhan. */
    I2C1->DR = (dev_addr << 1) | 1; 
    while(!(I2C1->SR1 & (1 << 1))); 
    (void)I2C1->SR1; 
    (void)I2C1->SR2; 
    /* Nhan tung byte khi RXNE bat; luu bo dem cho den khi con 3 byte cuoi. */
    uint8_t i = 0; 
    while(len > 3){ 
        while(!(I2C1->SR1 & (1 << 6))); 
        data[i] = I2C1->DR; 
        i++; len--; 
    }
    while(!(I2C1->SR1 & (1 << 2))); 
    /* Xu ly 3 byte cuoi: sau BTF, tat ACK, doc DR, phat STOP va lay cac byte con lai. */
    I2C1->CR1 &= ~(1 << 10); 
    data[i++] = I2C1->DR; 
    I2C1->CR1 |= (1 << 9); 
    data[i++] = I2C1->DR; 
    while (!(I2C1->SR1 & (1 << 6))); 
    data[i] = I2C1->DR; 
    
    if (len <= 2) 
    return; 
}
