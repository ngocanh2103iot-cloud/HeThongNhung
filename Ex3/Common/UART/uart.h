#ifndef UART_H 
#define UART_H 

#include <stdint.h> 

/* Khoi tao USART1 de gui du lieu. */
void UART_Init(void); 
void UART_Send_Char(char c); 
void UART_Send_String(const char *string); 
void UART_Send_U32(uint32_t value); 
void UART_Send_U16_Buffer(const uint16_t *buffer, uint16_t length);
/* Vi du: 2534 voi 2 chu so thap phan se in 25.34. */
void UART_Send_Fixed(int32_t value, uint8_t decimal_digits); 

#endif 
