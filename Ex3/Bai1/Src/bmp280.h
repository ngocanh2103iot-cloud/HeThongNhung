#ifndef BMP280_H
#define BMP280_H

#include "i2c.h"

/* Tra ve 1 khi khoi tao thanh cong, 0 khi loi. */
uint8_t Bmp280_Init(void);
/* Doc nhiet do: 0.01 do C, ap suat: Pa. */
void Bmp280_Read(int32_t *temperature, uint32_t *pressure);

#endif
