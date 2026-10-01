#include "stm32f1xx.h" 
#include "max7219.h" 
#include "spi.h" 
int main(void) 
{
    /* Khoi tao bus SPI va ma tran LED. */
    SPI_Init(); 
    Max7219_Init(); 
    /* Vi tri va huong dich hinh tim. */
    int8_t offset = 0; 
    int8_t direction = -1; 

    while (1) 
    {
        /* Ve khung hinh tai vi tri hien tai. */
        Max7219_Heart(offset); 

        /* Tre tam bang vong lap de thay chuyen dong. */
        for (volatile uint32_t i = 0; i < 800000; i++); 

        /* Dich vi tri theo huong hien tai; dao huong tai hai bien -2 va 0. */
        offset += direction;

        if (offset <= -2) 
        {
            direction = 1;      
        }
        else if (offset >= 0) 
        {
            direction = -1;     
        }
    }
}
