#ifndef BMP280_H
#define BMP280_H

#include "i2c.h"
#define BMP280_ADDR        0x76

#define BMP280_REG_ID      0xD0
#define BMP280_REG_RESET   0xE0
#define BMP280_REG_STATUS  0xF3
#define BMP280_REG_CTRL    0xF4
#define BMP280_REG_CONFIG  0xF5
/* Tra ve 1 khi khoi tao thanh cong, 0 khi loi. */
uint8_t Bmp280_Init(void);
/* Doc nhiet do: 0.01 do C, ap suat: Pa. */
void Bmp280_Read(int32_t *temperature, uint32_t *pressure);

#endif
