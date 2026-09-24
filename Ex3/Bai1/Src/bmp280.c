#include "stm32f1xx.h"
#include "bmp280.h"

#define BMP280_ADDRESS_LOW       0x76U
#define BMP280_ADDRESS_HIGH      0x77U
#define BMP280_CHIP_ID           0x58U

#define BMP280_REG_CALIB         0x88U
#define BMP280_REG_ID            0xD0U
#define BMP280_REG_CTRL_MEAS     0xF4U
#define BMP280_REG_DATA          0xF7U

#define I2C_TIMEOUT              100000U

static uint8_t bmp_address;
static uint16_t dig_t1;
static int16_t dig_t2;
static int16_t dig_t3;
static uint16_t dig_p1;
static int16_t dig_p2;
static int16_t dig_p3;
static int16_t dig_p4;
static int16_t dig_p5;
static int16_t dig_p6;
static int16_t dig_p7;
static int16_t dig_p8;
static int16_t dig_p9;
static int32_t t_fine;

static void I2c_Init(void)
{
    RCC->APB2ENR |= (1 << 3);
    RCC->APB1ENR |= (1 << 21);

    /* PB6: SCL, PB7: SDA, alternate function open-drain. */
    GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
    GPIOB->CRL |= (0xF << 24) | (0xF << 28);

    I2C1->CR1 = 0U;

    /* PCLK1 = 8 MHz, I2C standard mode = 100 kHz. */
    I2C1->CR2 = 8U;
    I2C1->CCR = 40U;
    I2C1->TRISE = 9U;
    I2C1->CR1 = (1 << 10) | (1 << 0);
}

static uint8_t I2c_Wait_Set(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t timeout = I2C_TIMEOUT;

    while ((*reg & mask) == 0U)
    {
        if (timeout == 0U)
        {
            return 0U;
        }
        timeout--;
    }

    return 1U;
}

static uint8_t I2c_Wait_Clear(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t timeout = I2C_TIMEOUT;

    while ((*reg & mask) != 0U)
    {
        if (timeout == 0U)
        {
            return 0U;
        }
        timeout--;
    }

    return 1U;
}

static void I2c_Clear_Addr(void)
{
    volatile uint32_t value;

    value = I2C1->SR1;
    value = I2C1->SR2;
    (void)value;
}

static void I2c_Stop_Error(void)
{
    I2C1->CR1 |= (1 << 9);
    I2C1->SR1 &= ~((1 << 8) | (1 << 9) | (1 << 10) | (1 << 11));
    I2C1->CR1 &= ~(1 << 11);
    I2C1->CR1 |= (1 << 10);
}

static uint8_t I2c_Wait_Address(void)
{
    uint32_t timeout = I2C_TIMEOUT;

    while ((I2C1->SR1 & (1 << 1)) == 0U)
    {
        if ((I2C1->SR1 & (1 << 10)) != 0U)
        {
            I2C1->SR1 &= ~(1 << 10);
            I2C1->CR1 |= (1 << 9);
            return 0U;
        }

        if (timeout == 0U)
        {
            I2c_Stop_Error();
            return 0U;
        }
        timeout--;
    }

    return 1U;
}

static uint8_t I2c_Start(uint8_t address, uint8_t read)
{
    I2C1->CR1 |= (1 << 8);

    if (I2c_Wait_Set(&I2C1->SR1, (1 << 0)) == 0U)
    {
        I2c_Stop_Error();
        return 0U;
    }

    I2C1->DR = (uint32_t)((address << 1) | read);

    if (I2c_Wait_Address() == 0U)
    {
        return 0U;
    }

    return 1U;
}

static uint8_t I2c_Write_Register(uint8_t address, uint8_t reg,
                                  uint8_t value)
{
    if (I2c_Wait_Clear(&I2C1->SR2, (1 << 1)) == 0U)
    {
        return 0U;
    }

    if (I2c_Start(address, 0U) == 0U)
    {
        return 0U;
    }

    I2c_Clear_Addr();
    I2C1->DR = reg;

    if (I2c_Wait_Set(&I2C1->SR1, (1 << 7)) == 0U)
    {
        I2c_Stop_Error();
        return 0U;
    }

    I2C1->DR = value;

    if (I2c_Wait_Set(&I2C1->SR1, (1 << 2)) == 0U)
    {
        I2c_Stop_Error();
        return 0U;
    }

    I2C1->CR1 |= (1 << 9);
    return 1U;
}

static uint8_t I2c_Read_Registers(uint8_t address, uint8_t reg,
                                  uint8_t *data, uint8_t length)
{
    uint8_t remaining = length;

    if ((data == 0) || (length == 0U))
    {
        return 0U;
    }

    if (I2c_Wait_Clear(&I2C1->SR2, (1 << 1)) == 0U)
    {
        return 0U;
    }

    if (I2c_Start(address, 0U) == 0U)
    {
        return 0U;
    }

    I2c_Clear_Addr();
    I2C1->DR = reg;

    if (I2c_Wait_Set(&I2C1->SR1, (1 << 2)) == 0U)
    {
        I2c_Stop_Error();
        return 0U;
    }

    I2C1->CR1 |= (1 << 10);
    I2C1->CR1 &= ~(1 << 11);

    if (I2c_Start(address, 1U) == 0U)
    {
        return 0U;
    }

    if (remaining == 1U)
    {
        I2C1->CR1 &= ~(1 << 10);
        I2c_Clear_Addr();
        I2C1->CR1 |= (1 << 9);

        if (I2c_Wait_Set(&I2C1->SR1, (1 << 6)) == 0U)
        {
            I2c_Stop_Error();
            return 0U;
        }

        data[0] = (uint8_t)I2C1->DR;
    }
    else if (remaining == 2U)
    {
        I2C1->CR1 |= (1 << 11);
        I2C1->CR1 &= ~(1 << 10);
        I2c_Clear_Addr();

        if (I2c_Wait_Set(&I2C1->SR1, (1 << 2)) == 0U)
        {
            I2c_Stop_Error();
            return 0U;
        }

        I2C1->CR1 |= (1 << 9);
        data[0] = (uint8_t)I2C1->DR;
        data[1] = (uint8_t)I2C1->DR;
    }
    else
    {
        I2c_Clear_Addr();

        while (remaining > 3U)
        {
            if (I2c_Wait_Set(&I2C1->SR1, (1 << 6)) == 0U)
            {
                I2c_Stop_Error();
                return 0U;
            }

            *data = (uint8_t)I2C1->DR;
            data++;
            remaining--;
        }

        if (I2c_Wait_Set(&I2C1->SR1, (1 << 2)) == 0U)
        {
            I2c_Stop_Error();
            return 0U;
        }

        I2C1->CR1 &= ~(1 << 10);
        *data = (uint8_t)I2C1->DR;
        data++;

        if (I2c_Wait_Set(&I2C1->SR1, (1 << 2)) == 0U)
        {
            I2c_Stop_Error();
            return 0U;
        }

        I2C1->CR1 |= (1 << 9);
        *data = (uint8_t)I2C1->DR;
        data++;
        *data = (uint8_t)I2C1->DR;
    }

    I2C1->CR1 &= ~(1 << 11);
    I2C1->CR1 |= (1 << 10);
    return 1U;
}

static uint16_t Read_U16(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static int16_t Read_S16(const uint8_t *data)
{
    return (int16_t)Read_U16(data);
}

static int32_t Compensate_Temperature(int32_t adc_temperature)
{
    int32_t var1;
    int32_t var2;

    var1 = ((((adc_temperature >> 3) - ((int32_t)dig_t1 << 1))) *
            (int32_t)dig_t2) >> 11;
    var2 = (((((adc_temperature >> 4) - (int32_t)dig_t1) *
              ((adc_temperature >> 4) - (int32_t)dig_t1)) >> 12) *
            (int32_t)dig_t3) >> 14;

    t_fine = var1 + var2;
    return (t_fine * 5 + 128) >> 8;
}

static uint32_t Compensate_Pressure(int32_t adc_pressure)
{
    int64_t var1;
    int64_t var2;
    int64_t pressure;

    var1 = (int64_t)t_fine - 128000;
    var2 = var1 * var1 * (int64_t)dig_p6;
    var2 += (var1 * (int64_t)dig_p5) << 17;
    var2 += (int64_t)dig_p4 << 35;
    var1 = ((var1 * var1 * (int64_t)dig_p3) >> 8) +
           ((var1 * (int64_t)dig_p2) << 12);
    var1 = (((((int64_t)1 << 47) + var1) * (int64_t)dig_p1) >> 33);

    if (var1 == 0)
    {
        return 0U;
    }

    pressure = 1048576 - adc_pressure;
    pressure = (((pressure << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)dig_p9 * (pressure >> 13) *
            (pressure >> 13)) >> 25;
    var2 = ((int64_t)dig_p8 * pressure) >> 19;
    pressure = ((pressure + var1 + var2) >> 8) +
               ((int64_t)dig_p7 << 4);

    return (uint32_t)(pressure >> 8);
}

uint8_t Bmp280_Init(void)
{
    uint8_t id;
    uint8_t calibration[24];

    I2c_Init();

    bmp_address = BMP280_ADDRESS_LOW;
    if ((I2c_Read_Registers(bmp_address, BMP280_REG_ID, &id, 1U) == 0U) ||
        (id != BMP280_CHIP_ID))
    {
        bmp_address = BMP280_ADDRESS_HIGH;
        if ((I2c_Read_Registers(bmp_address, BMP280_REG_ID, &id, 1U) == 0U) ||
            (id != BMP280_CHIP_ID))
        {
            return 0U;
        }
    }

    if (I2c_Read_Registers(bmp_address, BMP280_REG_CALIB,
                           calibration, sizeof(calibration)) == 0U)
    {
        return 0U;
    }

    dig_t1 = Read_U16(&calibration[0]);
    dig_t2 = Read_S16(&calibration[2]);
    dig_t3 = Read_S16(&calibration[4]);
    dig_p1 = Read_U16(&calibration[6]);
    dig_p2 = Read_S16(&calibration[8]);
    dig_p3 = Read_S16(&calibration[10]);
    dig_p4 = Read_S16(&calibration[12]);
    dig_p5 = Read_S16(&calibration[14]);
    dig_p6 = Read_S16(&calibration[16]);
    dig_p7 = Read_S16(&calibration[18]);
    dig_p8 = Read_S16(&calibration[20]);
    dig_p9 = Read_S16(&calibration[22]);

    if (dig_p1 == 0U)
    {
        return 0U;
    }

    /* Nhiet do x1, ap suat x1, che do normal. */
    return I2c_Write_Register(bmp_address, BMP280_REG_CTRL_MEAS, 0x27U);
}

uint8_t Bmp280_Read(int32_t *temperature, uint32_t *pressure)
{
    uint8_t data[6];
    int32_t adc_temperature;
    int32_t adc_pressure;

    if ((temperature == 0) || (pressure == 0))
    {
        return 0U;
    }

    if (I2c_Read_Registers(bmp_address, BMP280_REG_DATA,
                           data, sizeof(data)) == 0U)
    {
        return 0U;
    }

    adc_pressure = ((int32_t)data[0] << 12) |
                   ((int32_t)data[1] << 4) |
                   ((int32_t)data[2] >> 4);
    adc_temperature = ((int32_t)data[3] << 12) |
                      ((int32_t)data[4] << 4) |
                      ((int32_t)data[5] >> 4);

    if ((adc_temperature == 0x80000) || (adc_pressure == 0x80000))
    {
        return 0U;
    }

    *temperature = Compensate_Temperature(adc_temperature);
    *pressure = Compensate_Pressure(adc_pressure);
    return 1U;
}
