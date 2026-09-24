#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

uint8_t Bmp280_Init(void);
uint8_t Bmp280_Read(int32_t *temperature, uint32_t *pressure);

#endif
