#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>

// Khởi tạo MAX7219
void max7219_init(void);

// Điều khiển một LED
void matrix_set(uint8_t x, uint8_t y, uint8_t state);  // 0: tắt, 1: bật

// Xóa màn hình
void matrix_clear(void);

// Gửi bộ đệm đến MAX7219 sau khi vẽ
void matrix_update(void);

// Gửi dữ liệu thô đến MAX7219
void max7219_send(uint8_t addr, uint8_t data);

#endif
