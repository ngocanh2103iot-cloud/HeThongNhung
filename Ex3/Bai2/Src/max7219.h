#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>

// Khoi tao MAX7219
void max7219_init(void);

// Dieu khien mot LED
void matrix_set(uint8_t x, uint8_t y, uint8_t state);  // 0: tat, 1: bat

// Xoa man hinh
void matrix_clear(void);

// Gui bo dem den MAX7219
void matrix_update(void);

// Gui du lieu tho den MAX7219
void max7219_send(uint8_t addr, uint8_t data);

#endif
