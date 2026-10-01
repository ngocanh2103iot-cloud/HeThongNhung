#include "stm32f1xx.h" 
#include "bmp280.h" 
#include "i2c.h" 
#include "uart.h" 
#include "delay.h" 

int main(void) 
{
    int32_t temperature; 
    uint32_t pressure; 

    /* Khoi tao giao tiep cam bien va cong gui ket qua. */
    I2C_Init(); 
    UART_Init(); 

    UART_Send_String("BMP280 start\r\n"); 

    /* Dung neu khoi tao cam bien that bai. */
    if (Bmp280_Init() == 0) 
    {
        UART_Send_String("BMP280 not found\r\n"); 

        while (1) 
        {
        }
    }

    /* Khoi tao bo dem tre dung SysTick. */
    Delay_Init(); 

    while (1) 
    {
        /* Cho 1 giay truoc moi lan doc. */
        Delay_ms(1000); 
        /* Doc ket qua da bu va gui qua UART. */
        Bmp280_Read(&temperature, &pressure); 
        /* Gui nhiet do voi 2 chu so thap phan, don vi do C. */
        UART_Send_String("Temperature: "); 
        UART_Send_Fixed(temperature, 2U); 
        UART_Send_String(" C\r\n"); 

        /* Gui ap suat nguyen theo Pa, roi xuong dong de tach ban tin. */
        UART_Send_String("Pressure: "); 
        UART_Send_U32(pressure); 
        UART_Send_String(" Pa\r\n\r\n"); 
    }
}
