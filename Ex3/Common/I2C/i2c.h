#ifndef I2C_H 
#define I2C_H 

#include "stm32f1xx.h" 
#include <stdint.h> 

/* Khoi tao I2C1 va doc/ghi thanh ghi cua thiet bi. */
void I2C_Init(void); 
void I2C_WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t data); 
uint8_t I2C_ReadRegister(uint8_t dev_addr, uint8_t reg_addr); 
void I2C_ReadRegisters(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len); 
#endif 
