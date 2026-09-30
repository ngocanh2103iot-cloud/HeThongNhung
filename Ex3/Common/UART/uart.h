#ifndef UART_H
#define UART_H

#include <stdint.h>

/* Khoi tao USART1 de gui du lieu. */
void Uart_Init(void);
void Uart_Send_Char(char c);
void Uart_Send_String(const char *string);
void Uart_Send_U32(uint32_t value);
/* Vi du: 2534 voi 2 chu so thap phan se in 25.34. */
void Uart_Send_Fixed(int32_t value, uint8_t decimal_digits);

#endif
