#include "max7219.h"
void Max7219_SendData(uint8_t addr, uint8_t data){
    GPIOA->ODR &= ~(1 << 4);

    Spi_TransferByte(addr);
    Spi_TransferByte(data);
    while(SPI1->SR & (1 << 7));

    GPIOA->ODR |= (1 << 4);
}
void Max7219_Init(){
    Max7219_SendData(0x09, 0x00);
    Max7219_SendData(0x0B, 0x07);
    Max7219_SendData(0x0A, 0x00);
    Max7219_SendData(0x0F, 0x00);

    for (int i=1; i<=8; i++)
        Max7219_SendData(i, 0x00);

    Max7219_SendData(0x0c, 0x01);
}
uint8_t matrix[8];
void Max7219_Set(int x, int y, int state)
{
    uint8_t row = 7 - x;
    uint8_t bit = 7 - y;
    if (state)
        matrix[row] |= (1 << bit);
    else
        matrix[row] &= ~(1 << bit);
}
void Max7219_Clear()
{
    for (int i=0; i<8; i++)
        matrix[i] = 0;
}
void Max7219_Update(){
    for(int i=0; i<8; i++)
        Max7219_SendData(i + 1, matrix[i]);
}
void Max7219_Heart(int8_t offset_y)
{
    static const uint8_t heart[8] = {
        0b00000000,
        0b01100110,
        0b11111111,
        0b11111111,
        0b11111111,
        0b01111110,
        0b00111100,
        0b00011000
    };

    /* Clear buffer trước khi tạo frame mới */
    for (uint8_t i = 0; i < 8; i++)
    {
        matrix[i] = 0x00;
    }

    for (uint8_t y = 0; y < 8; y++)
    {
        for (uint8_t x = 0; x < 8; x++)
        {
            uint8_t state = (heart[y] >> (7 - x)) & 0x01;

            if (state)
            {
                int8_t new_y = y + offset_y;

                /* Chỉ vẽ pixel còn nằm trong matrix */
                if (new_y >= 0 && new_y < 8)
                {
                    Max7219_Set(x, new_y, 1);
                }
            }
        }
    }

    Max7219_Update();
}