#include "max7219.h"
#include "stm32f1xx.h"

// Bo dem 8 hang, moi bit la mot LED
static uint8_t display_buffer[8] = {0};

// Tao tre SPI bang phan mem
static void spi_delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 10; i++);
}

// Gui du lieu bang SPI
static void spi_transmit(uint8_t data)
{
    // Cho bo dem truyen trong
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    // Cho truyen xong
    while (SPI1->SR & SPI_SR_BSY);
}

// Dieu khien chan CS
static void cs_low(void)
{
    GPIOA->BRR = GPIO_BRR_BR4;
}

static void cs_high(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS4;
}

// Gui lenh 16 bit den MAX7219
void max7219_send(uint8_t addr, uint8_t data)
{
    cs_low();
    spi_delay();
    spi_transmit(addr);
    spi_transmit(data);
    spi_delay();
    cs_high();
}

// Khoi tao GPIO va SPI cho MAX7219
void max7219_init(void)
{
    // Cap xung nhip cho SPI1 va GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;

    // PA4: CS, ngo ra day-keo
    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1;  // Ngo ra 2 MHz

    // PA5: SCK, PA7: MOSI, chuc nang thay the day-keo
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_CNF5_1;  // AF day-keo 2 MHz
    GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;  // AF day-keo 2 MHz

    // SPI1 che do chu, cuc tinh thap, lay mau canh dau
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SPE;  // Bat SPI

    // Cau hinh MAX7219
    max7219_send(0x09, 0x00);    // Khong giai ma
    max7219_send(0x0A, 0x07);    // Do sang 0x07
    max7219_send(0x0B, 0x07);    // Quet du 8 hang
    max7219_send(0x0C, 0x01);    // Hoat dong binh thuong
    max7219_send(0x0F, 0x00);    // Tat che do kiem tra

    matrix_clear();
    matrix_update();
}

// Cap nhat man hinh tu bo dem
void matrix_update(void)
{
    for (int row = 1; row <= 8; row++) {
        max7219_send(row, display_buffer[row - 1]);
    }
}

// Dat LED tai (x, y) trong bo dem
// x: cot, y: hang, trang thai: 0 tat, 1 bat
void matrix_set(uint8_t x, uint8_t y, uint8_t state)
{
    if (x > 7 || y > 7) return;
    if (state) {
        display_buffer[y] |= (1 << x);
    } else {
        display_buffer[y] &= ~(1 << x);
    }
}

// Giu tuong thich cu
void matrix_on(uint8_t x, uint8_t y) { matrix_set(x, y, 1); }
void matrix_off(uint8_t x, uint8_t y) { matrix_set(x, y, 0); }

// Xoa toan bo LED trong bo dem
void matrix_clear(void)
{
    for (int i = 0; i < 8; i++) {
        display_buffer[i] = 0;
    }
}
