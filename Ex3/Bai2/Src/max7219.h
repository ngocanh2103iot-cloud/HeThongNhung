#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>

// Initialize MAX7219
void max7219_init(void);

// Control a single LED
void matrix_set(uint8_t x, uint8_t y, uint8_t state);  // state: 0=off, 1=on

// Clear display
void matrix_clear(void);

// Send display buffer to MAX7219 (call after drawing)
void matrix_update(void);

// Send raw data to MAX7219
void max7219_send(uint8_t addr, uint8_t data);

#endif
