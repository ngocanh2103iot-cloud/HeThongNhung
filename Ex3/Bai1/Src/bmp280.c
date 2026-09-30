#include "bmp280.h"

/* He so hieu chinh va gia tri nhiet do trung gian. */
int32_t dig_T[3];
int32_t dig_P[9];
int32_t t_fine;

/* Doc 24 byte he so; T1 va P1 la so khong dau. */
void Bmp280_ReadCalibration(){
    uint8_t calib[24];
    I2C_ReadRegisters(0x76, 0x88, calib, 24);

    for (uint8_t i = 0; i < 3; i++){
        uint16_t value;
        value = (calib[i * 2 + 1] << 8) | calib[i * 2];
        if (i == 0)
            dig_T[i] = value;
        else
            dig_T[i] = (int16_t)value;
    }
    for (uint8_t i = 0; i < 9; i++){
        uint8_t index = 6 + i * 2;
        uint16_t value;
        value = (calib[index + 1] << 8) | calib[index];
        if (i == 0)
            dig_P[i] = value;            
        else
            dig_P[i] = (int16_t)value;   
    }
}
/* Ghep 6 byte thanh ap suat va nhiet do tho 20 bit. */
void Bmp280_ReadRawData(int32_t *adc_T, int32_t *adc_P){
    uint8_t raw[6];

    I2C_ReadRegisters(0x76, 0xF7, raw, 6);
    *adc_P = (raw[0] << 12) | (raw[1] << 4) | (raw[2] >> 4);
    *adc_T = (raw[3] << 12) | (raw[4] << 4) | (raw[5] >> 4);
}
uint8_t Bmp280_Init(){
    uint8_t id;
    uint8_t status;
    /* Kiem tra ID truoc khi reset cam bien. */
    id = I2C_ReadRegister(0x76, 0xD0);
    if(id != 0x58) 
        return 0;
    I2C_WriteRegister(0x76, 0xE0, 0xB6); 
    /* Cho cam bien nap xong he so hieu chinh. */
    do{
        status = I2C_ReadRegister(0x76, 0xF3);
    }
    while (status & 1);
    Bmp280_ReadCalibration();
    I2C_WriteRegister(0x76, 0xF5, 0x00);
    I2C_WriteRegister(0x76, 0xF4, 0x2F);
    return 1;
}

/* Bu nhiet do, tra ve don vi 0.01 do C va cap nhat t_fine. */
int32_t Bmp280_Compensation_T(int32_t adc_T){
    int32_t var1;
    int32_t var2;
    int32_t T;

    var1 =((((adc_T >> 3) - (dig_T[0] << 1))) * dig_T[1]) >> 11;
    var2 =(((((adc_T >> 4) - dig_T[0]) * ((adc_T >> 4) - dig_T[0])) >> 12) * dig_T[2]) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}
/* Bu ap suat theo t_fine, tra ve don vi 1/256 Pa. */
uint32_t Bmp280_Compensation_P(int32_t adc_P)
{
    int64_t var1;
    int64_t var2;
    int64_t p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P[5];
    var2 = var2 + ((var1 * (int64_t)dig_P[4]) << 17);
    var2 = var2 + (((int64_t)dig_P[3]) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P[2]) >> 8) + ((var1 * (int64_t)dig_P[1]) << 12);
    var1 = (((((int64_t)1) << 47) + var1) * ((int64_t)dig_P[0])) >> 33;

    /* Tranh chia cho 0. */
    if (var1 == 0)
    {
        return 0;
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P[8]) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P[7]) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P[6]) << 4);

    return (uint32_t)p;
}
void Bmp280_Read(int32_t *temperature, uint32_t *pressure)
{
    int32_t adc_T;
    int32_t adc_P;

    Bmp280_ReadRawData(&adc_T, &adc_P);

    /* Bu nhiet do truoc de co t_fine cho bu ap suat. */
    *temperature = Bmp280_Compensation_T(adc_T);

    *pressure = Bmp280_Compensation_P(adc_P) / 256;
}