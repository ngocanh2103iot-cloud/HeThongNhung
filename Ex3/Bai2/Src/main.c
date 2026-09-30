#include "stm32f1xx.h"
#include "max7219.h"
#include "spi.h"
int main(void)
{
    Spi_Init();
    Max7219_Init();
    int8_t offset = 0;
    int8_t direction = -1;

    while (1)
    {
        Max7219_Heart(offset);

        /* delay tạm để nhìn thấy animation */
        for (volatile uint32_t i = 0; i < 800000; i++);

        offset += direction;

        if (offset <= -2)
        {
            direction = 1;      // chuyển sang đi xuống
        }
        else if (offset >= 0)
        {
            direction = -1;     // chuyển sang đi lên
        }
    }
}
