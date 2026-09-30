#include "stm32f1xx.h"
#include "bmp280.h"
#include "i2c.h"
#include "uart.h"

/* SysTick: 72 MHz / 8 = 9 MHz, moi chu ky 1 giay. */
static void Systick_Init(void)
{
    SysTick->LOAD = 9000000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = (1 << 0);
}

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

    Systick_Init();

    while (1)
    {
        /* Cho COUNTFLAG (bit 16), doc va gui ket qua moi giay. */
        while ((SysTick->CTRL & (1 << 16)) == 0)
        {
        }
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
