#include "stm32f1xx.h"
#include "uart.h"

void Uart_Init(void)
{
    /* Cap clock GPIOA (bit 2) va USART1 (bit 14). */
    RCC->APB2ENR |= (1 << 2) | (1 << 14);

    /* PA9: USART1_TX, chuc nang thay the day-keo 50 MHz. */
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |= (0xB << 4);

    /* PCLK2 = 72 MHz, 72000000 / 9600 = 7500. */
    USART1->BRR = 7500;
    /* TE (bit 3): bat truyen; UE (bit 13): bat USART. */
    USART1->CR1 = (1 << 3) | (1 << 13);
}

/* Cho TXE (bit 7) bao DR trong roi gui ky tu. */
void Uart_Send_Char(char c)
{
    while ((USART1->SR & (1 << 7)) == 0U)
    {
    }

    USART1->DR = (uint8_t)c;
}

/* Gui chuoi den ky tu ket thuc. */
void Uart_Send_String(const char *string)
{
    while (*string != '\0')
    {
        Uart_Send_Char(*string);
        string++;
    }
}

/* Tach chu so va gui so nguyen khong dau. */
void Uart_Send_U32(uint32_t value)
{
    char buffer[10];
    uint8_t i = 0U;

    if (value == 0U)
    {
        Uart_Send_Char('0');
        return;
    }

    while (value > 0U)
    {
        buffer[i] = (char)('0' + (value % 10U));
        value /= 10U;
        i++;
    }

    while (i > 0U)
    {
        i--;
        Uart_Send_Char(buffer[i]);
    }
}

/* In so nguyen theo so chu so thap phan chi dinh. */
void Uart_Send_Fixed(int32_t value, uint8_t decimal_digits)
{
    uint32_t divisor = 1U;
    uint32_t absolute_value;
    uint8_t i;

    if (value < 0)
    {
        Uart_Send_Char('-');
        absolute_value = (uint32_t)(-(int64_t)value);
    }
    else
    {
        absolute_value = (uint32_t)value;
    }

    for (i = 0U; i < decimal_digits; i++)
    {
        divisor *= 10U;
    }

    Uart_Send_U32(absolute_value / divisor);

    if (decimal_digits == 0U)
    {
        return;
    }

    Uart_Send_Char('.');
    divisor /= 10U;

    while (divisor > 0U)
    {
        Uart_Send_Char((char)('0' + ((absolute_value / divisor) % 10U)));
        divisor /= 10U;
    }
}
