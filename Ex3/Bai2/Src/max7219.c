#include "max7219.h" 
/* Gui dia chi va du lieu trong mot lan keo CS xuong. */
void Max7219_SendData(uint8_t addr, uint8_t data){ 
    /* Keo CS xuong va gui 2 byte: dia chi thanh ghi truoc, du lieu sau. */
    GPIOA->ODR &= ~(1 << 4); 

    SPI_TransferByte(addr); 
    SPI_TransferByte(data); 
    /* Cho SPI het ban truoc khi chot du lieu. */
    while(SPI1->SR & (1 << 7)); 

    GPIOA->ODR |= (1 << 4); 
}
/* Cai dat che do hien thi va xoa 8 hang LED. */
void Max7219_Init(){ 
    /* Tat giai ma BCD, quet 8 hang, chon do sang thap nhat va tat che do test. */
    Max7219_SendData(0x09, 0x00); 
    Max7219_SendData(0x0B, 0x07); 
    Max7219_SendData(0x0A, 0x00); 
    Max7219_SendData(0x0F, 0x00); 

    /* Xoa du lieu ca 8 hang truoc khi thoat shutdown va bat hien thi. */
    for (int i=1; i<=8; i++) 
        Max7219_SendData(i, 0x00); 

    Max7219_SendData(0x0c, 0x01); 
}
/* Bo dem 8 hang, moi bit ung voi mot LED. */
uint8_t matrix[8]; 
void Max7219_Set(int x, int y, int state) 
{
    /* Doi toa do ve vi tri hang va bit trong bo dem. */
    uint8_t row = 7 - x; 
    uint8_t bit = 7 - y; 
    /* Dat hoac xoa bit LED trong bo dem; chua gui ra phan cung tai day. */
    if (state) 
        matrix[row] |= (1 << bit); 
    else 
        matrix[row] &= ~(1 << bit); 
}
/* Xoa bo dem; can Update de dua ra LED. */
void Max7219_Clear() 
{
    for (int i=0; i<8; i++) 
        matrix[i] = 0; 
}
/* Gui tung hang cua bo dem ra MAX7219. */
void Max7219_Update(){ 
    for(int i=0; i<8; i++) 
        Max7219_SendData(i + 1, matrix[i]); 
}
/* Dich hinh tim theo truc y va cap nhat LED. */
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

    /* Xoa bo dem truoc khi tao khung hinh moi. */
    for (uint8_t i = 0; i < 8; i++) 
    {
        matrix[i] = 0x00; 
    }

    /* Duyet tung diem cua mau tim; chi dich va ve cac diem co bit bang 1. */
    for (uint8_t y = 0; y < 8; y++) 
    {
        for (uint8_t x = 0; x < 8; x++) 
        {
            uint8_t state = (heart[y] >> (7 - x)) & 0x01;

            if (state) 
            {
                int8_t new_y = y + offset_y;

                /* Chi ve diem con nam trong ma tran. */
                if (new_y >= 0 && new_y < 8) 
                {
                    Max7219_Set(x, new_y, 1); 
                }
            }
        }
    }

    /* Gui khung hinh hoan chinh sau khi da ve xong bo dem. */
    Max7219_Update(); 
}
