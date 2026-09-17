#include "uart_init.h"
#include "stm32f1xx.h"

static volatile char buffer[64];
static volatile int buffer_index;
static volatile int message_ready;

static void UART_SendChar(char character)
{
    while ((USART1->SR & (1 << 7)) == 0U) {
    }
    USART1->DR = character;
}

static void UART_SendString(const char *string)
{
    while (*string != '\0') {
        UART_SendChar(*string++);
    }
}

static void UART_SendBuffer(void)
{
    int index = 0;

    while (buffer[index] != '\0') {
        UART_SendChar(buffer[index++]);
    }
}

void UART_Init(void)
{
    RCC->APB2ENR |= (1 << 2);
    RCC->APB2ENR |= (1 << 14);

    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |= (0xB << 4);

    GPIOA->CRH &= ~(0xF << 8);
    GPIOA->CRH |= (0x4 << 8);

    USART1->BRR = 7500;
    USART1->CR1 &= ~(1 << 12);
    USART1->CR1 |= (1 << 2);
    USART1->CR1 |= (1 << 3);
    USART1->CR1 |= (1 << 5);
    USART1->CR1 |= (1 << 13);

    NVIC_EnableIRQ(USART1_IRQn);
}

void UART_Process(void)
{
    if (message_ready) {
        UART_SendString("ELE1415_02: ");
        UART_SendBuffer();
        UART_SendString("\r\n");
        buffer_index = 0;
        message_ready = 0;
    }
}

void USART1_IRQHandler(void)
{
    char character = USART1->DR;

    if (character == '!') {
        buffer[buffer_index] = '\0';
        message_ready = 1;
    } else if (character != '\r' && character != '\n') {
        buffer[buffer_index++] = character;
    }
}
