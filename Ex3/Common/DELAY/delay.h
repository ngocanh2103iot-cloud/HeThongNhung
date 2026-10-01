#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

/* Dung rieng SysTick, khong dung chung voi bo dem SysTick khac. */
void Delay_Init(void);
/* Cho chan theo ms; goi sau Delay_Init, khong goi trong ngat. */
void Delay_ms(uint32_t ms);

#endif
