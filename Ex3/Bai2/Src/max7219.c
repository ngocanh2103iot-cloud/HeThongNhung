#include "max7219.h"
#include "stm32f1xx.h"

// Bộ đệm 8 hàng, mỗi bit là một LED
static uint8_t display_buffer[8] = {0};

// Tạo trễ SPI bằng phần mềm
static void spi_delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 10; i++);
}

// Gửi dữ liệu bằng SPI
static void spi_transmit(uint8_t data)
{
    // Chờ bộ đệm truyền trống
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    // Chờ truyền xong
    while (SPI1->SR & SPI_SR_BSY);
}

// Điều khiển chân CS
static void cs_low(void)
{
    GPIOA->BRR = GPIO_BRR_BR4;
}

static void cs_high(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS4;
}

// Gửi lệnh 16 bit đến MAX7219
void max7219_send(uint8_t addr, uint8_t data)
{
    cs_low();
    spi_delay();
    spi_transmit(addr);
    spi_transmit(data);
    spi_delay();
    cs_high();
}

// Khởi tạo GPIO và SPI cho MAX7219
void max7219_init(void)
{
    // Cấp xung nhịp cho SPI1 và GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;

    // PA4: CS, ngõ ra đẩy-kéo
    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1;  // Ngõ ra 2 MHz

    // PA5: SCK, PA7: MOSI, chức năng thay thế đẩy-kéo
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_CNF5_1;  // AF đẩy-kéo 2 MHz
    GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;  // AF đẩy-kéo 2 MHz

    // SPI1 chế độ chủ, cực tính thấp, lấy mẫu cạnh đầu
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SPE;  // Bật SPI

    // Cấu hình MAX7219
    max7219_send(0x09, 0x00);    // Không giải mã
    max7219_send(0x0A, 0x07);    // Độ sáng 0x07
    max7219_send(0x0B, 0x07);    // Quét đủ 8 hàng
    max7219_send(0x0C, 0x01);    // Hoạt động bình thường
    max7219_send(0x0F, 0x00);    // Tắt chế độ kiểm tra

    matrix_clear();
    matrix_update();
}

// Cập nhật màn hình từ bộ đệm
void matrix_update(void)
{
    for (int row = 1; row <= 8; row++) {
        max7219_send(row, display_buffer[row - 1]);
    }
}

// Đặt LED tại (x, y) trong bộ đệm
// x: cột, y: hàng, trạng thái: 0 tắt, 1 bật
void matrix_set(uint8_t x, uint8_t y, uint8_t state)
{
    if (x > 7 || y > 7) return;
    if (state) {
        display_buffer[y] |= (1 << x);
    } else {
        display_buffer[y] &= ~(1 << x);
    }
}

// Giữ tương thích cũ
void matrix_on(uint8_t x, uint8_t y) { matrix_set(x, y, 1); }
void matrix_off(uint8_t x, uint8_t y) { matrix_set(x, y, 0); }

// Xóa toàn bộ LED trong bộ đệm
void matrix_clear(void)
{
    for (int i = 0; i < 8; i++) {
        display_buffer[i] = 0;
    }
}
