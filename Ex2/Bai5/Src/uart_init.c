#include "uart_init.h"
#include "stm32f1xx.h"

volatile char buffer[64];
volatile int i = 0;
volatile int ready = 0;
volatile int rx_error = 0;

void Uart_Init(void){
    RCC->APB2ENR |= (1 << 2);
    RCC->APB2ENR |= (1 << 14);

    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |= (0xB << 4);       // PA9: USART1 TX
    GPIOA->CRH &= ~(0xF << 8);
    GPIOA->CRH |= (0x4 << 8);       // PA10: USART1 RX

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    USART1->BRR = 7500;            // 72 MHz / 9600, 8N1
    USART1->CR1 |= (1 << 2);
    USART1->CR1 |= (1 << 3);
    USART1->CR1 |= (1 << 5);
    USART1->CR1 |= (1 << 13);
    NVIC_EnableIRQ(USART1_IRQn);
}

void Send_Char(char a){
    while(!(USART1->SR & (1 << 7)));
    USART1->DR = a;
}

void Send_String(const char *string){
    while(*string != '\0'){
        Send_Char(*string);
        string++;
    }
}

void USART1_IRQHandler(void){
    uint32_t status = USART1->SR;
    char c;

    if(!(status & ((1 << 5) | 0xF))) return;
    c = (char)USART1->DR;           // Doc SR roi DR de xoa co RX/loi
    if(ready) return;              // Cho main xu ly lenh hien tai

    if(status & 0xF){              // PE, FE, NE, ORE: bo lenh bi loi
        rx_error = 1;
        return;
    }
    if(c == '!'){
        buffer[i] = '\0';
        ready = 1;
    }
    else if(c != '\r' && c != '\n'){
        if(rx_error) return;       // Bo phan con lai den dau '!'
        if(i < (int)sizeof(buffer) - 1){
            buffer[i] = c;
            i++;
        }
        else rx_error = 1;
    }
}
