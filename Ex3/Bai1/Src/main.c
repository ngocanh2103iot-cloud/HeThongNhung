#include "stm32f1xx.h"
#include "bmp280.h"
#include "uart.h"

static void Delay_Init(void)
{
    SysTick->LOAD = 8000U - 1U;
    SysTick->VAL = 0U;
    SysTick->CTRL = (1 << 2) | (1 << 0);
}

static void Delay_Ms(uint32_t time)
{
    while (time > 0U)
    {
        while ((SysTick->CTRL & (1 << 16)) == 0U)
        {
        }
        time--;
    }
}

int main(void)
{
    int32_t temperature;
    uint32_t pressure;

    Delay_Init();
    Uart_Init();

    Uart_Send_String("BMP280 start\r\n");

    if (Bmp280_Init() == 0U)
    {
        Uart_Send_String("BMP280 not found\r\n");

        while (1)
        {
        }
    }

    Delay_Ms(10U);

    while (1)
    {
        if (Bmp280_Read(&temperature, &pressure) != 0U)
        {
            Uart_Send_String("Temperature: ");
            Uart_Send_Fixed(temperature, 2U);
            Uart_Send_String(" C\r\n");

            Uart_Send_String("Pressure: ");
            Uart_Send_U32(pressure);
            Uart_Send_String(" Pa\r\n\r\n");
        }
        else
        {
            Uart_Send_String("BMP280 read error\r\n");
        }

        Delay_Ms(1000U);
    }
}
