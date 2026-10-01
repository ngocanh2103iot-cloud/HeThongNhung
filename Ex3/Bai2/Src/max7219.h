#ifndef MAX7219_H 
#define MAX7219_H 

#include <stdint.h> 
#include "stm32f1xx.h" 
#include "spi.h" 
// Khoi tao MAX7219
void Max7219_Init(void); 

// Dieu khien mot LED
void Max7219_Set(int x, int y, int state);  

// Xoa bo dem hien thi
void Max7219_Clear(void); 

// Gui bo dem den MAX7219
void Max7219_Update(void); 

// Gui du lieu tho den MAX7219
void Max7219_SendData(uint8_t addr, uint8_t data); 

/* Ve hinh tim voi do dich theo truc y. */
void Max7219_Heart(int8_t offset_y); 
#endif 
