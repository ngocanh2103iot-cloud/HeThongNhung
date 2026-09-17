#ifndef UART_INIT_H
#define UART_INIT_H

extern volatile char buffer[64];
extern volatile int i;
extern volatile int ready;
extern volatile int rx_error;

void Uart_Init(void);
void Send_Char(char a);
void Send_String(const char *string);

#endif
