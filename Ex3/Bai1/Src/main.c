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
    I2c_Init();
    Uart_Init();

    Uart_Send_String("BMP280 start\r\n");

    /* Dung neu khoi tao cam bien that bai. */
    if (Bmp280_Init() == 0)
    {
        Uart_Send_String("BMP280 not found\r\n");

        while (1)
        {
        }
    }

    Delay_Init();

    while (1)
    {
        Delay_ms(1000);
        /* Doc ket qua da bu va gui qua UART. */
        Bmp280_Read(&temperature, &pressure);
        Uart_Send_String("Temperature: ");
        Uart_Send_Fixed(temperature, 2U);
        Uart_Send_String(" C\r\n");

        Uart_Send_String("Pressure: ");
        Uart_Send_U32(pressure);
        Uart_Send_String(" Pa\r\n\r\n");
    }
}
