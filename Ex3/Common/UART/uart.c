#include "stm32f1xx.h" 
#include "uart.h" 

void UART_Init(void) 
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
void UART_Send_Char(char c) 
{
    while ((USART1->SR & (1 << 7)) == 0U) 
    {
    }

    USART1->DR = (uint8_t)c; 
}

/* Gui chuoi den ky tu ket thuc. */
void UART_Send_String(const char *string) 
{
    while (*string != '\0') 
    {
        UART_Send_Char(*string); 
        string++; 
    }
}

/* Tach chu so va gui so nguyen khong dau. */
void UART_Send_U32(uint32_t value) 
{
    char buffer[10]; 
    uint8_t i = 0U; 

    /* Xu ly rieng so 0 vi vong tach chu so ben duoi chi chay khi value > 0. */
    if (value == 0U) 
    {
        UART_Send_Char('0'); 
        return; 
    }

    /* Tach cac chu so tu hang don vi va luu nguoc vao bo dem 10 ky tu. */
    while (value > 0U) 
    {
        buffer[i] = (char)('0' + (value % 10U));
        value /= 10U;
        i++; 
    }

    /* Gui nguoc bo dem de chu so hang cao xuat hien truoc. */
    while (i > 0U) 
    {
        i--; 
        UART_Send_Char(buffer[i]); 
    }
}

/* Gui cac phan tu 16 bit duoi dang so, phan cach bang \n\r. */
void UART_Send_U16_Buffer(const uint16_t *buffer, uint16_t length)
{
    uint16_t i;

    for (i = 0U; i < length; i++)
    {
        UART_Send_U32(buffer[i]);
        UART_Send_Char('\n');
        UART_Send_Char('\r');
    }
}

/* In so nguyen theo so chu so thap phan chi dinh. */
void UART_Send_Fixed(int32_t value, uint8_t decimal_digits) 
{
    uint32_t divisor = 1U; 
    uint32_t absolute_value; 
    uint8_t i; 

    /* Gui dau tru neu can va lay do lon; dung int64_t khi doi dau de tranh tran. */
    if (value < 0) 
    {
        UART_Send_Char('-'); 
        absolute_value = (uint32_t)(-(int64_t)value);
    }
    else 
    {
        absolute_value = (uint32_t)value;
    }

    /* Tao bo chia theo so chu so thap phan, sau do gui phan nguyen. */
    for (i = 0U; i < decimal_digits; i++) 
    {
        divisor *= 10U;
    }

    UART_Send_U32(absolute_value / divisor); 

    /* Neu khong co phan thap phan thi dung; nguoc lai gui dau cham. */
    if (decimal_digits == 0U) 
    {
        return; 
    }

    UART_Send_Char('.'); 
    divisor /= 10U;

    /* Gui tung chu so thap phan, ke ca so 0 o dau hoac cuoi. */
    while (divisor > 0U) 
    {
        UART_Send_Char((char)('0' + ((absolute_value / divisor) % 10U))); 
        divisor /= 10U;
    }
}
