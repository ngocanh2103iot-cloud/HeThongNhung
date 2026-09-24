#include "max7219.h"
#include "stm32f1xx.h"

// Display buffer: 8 rows (8 bytes), each bit represents one LED
static uint8_t display_buffer[8] = {0};

// SPI delay (simple software delay)
static void spi_delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 10; i++);
}

// Hardware SPI transmit
static void spi_transmit(uint8_t data)
{
    // Wait until TX buffer empty
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    // Wait for transmission complete
    while (SPI1->SR & SPI_SR_BSY);
}

// CS low/high
static void cs_low(void)
{
    GPIOA->BRR = GPIO_BRR_BR4;
}

static void cs_high(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS4;
}

// Send 16-bit command to MAX7219
void max7219_send(uint8_t addr, uint8_t data)
{
    cs_low();
    spi_delay();
    spi_transmit(addr);
    spi_transmit(data);
    spi_delay();
    cs_high();
}

// Initialize GPIO and SPI for MAX7219
void max7219_init(void)
{
    // Enable SPI1 and GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;

    // PA4 = CS (output push-pull)
    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1;  // 2MHz output

    // PA5 = SCK, PA7 = MOSI (alternate function push-pull)
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |= GPIO_CRL_MODE5_1 | GPIO_CRL_CNF5_1;  // AF PP 2MHz
    GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;  // AF PP 2MHz

    // SPI1 configuration: Master, Polarity Low, Phase 1 Edge
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
    SPI1->CR1 |= SPI_CR1_SPE;  // Enable SPI

    // MAX7219 initialization sequence
    max7219_send(0x09, 0x00);    // Decode mode: no decode
    max7219_send(0x0A, 0x07);    // Intensity: max (0x00-0x0F)
    max7219_send(0x0B, 0x07);    // Scan limit: all 8 digits
    max7219_send(0x0C, 0x01);    // Shutdown: normal operation
    max7219_send(0x0F, 0x00);   // Test mode: off

    matrix_clear();
    matrix_update();
}

// Update display from buffer (public function)
void matrix_update(void)
{
    for (int row = 1; row <= 8; row++) {
        max7219_send(row, display_buffer[row - 1]);
    }
}

// Set LED state at (x, y) - only updates buffer
// x = column (0-7), y = row (0-7), state: 0=off, 1=on
void matrix_set(uint8_t x, uint8_t y, uint8_t state)
{
    if (x > 7 || y > 7) return;
    if (state) {
        display_buffer[y] |= (1 << x);
    } else {
        display_buffer[y] &= ~(1 << x);
    }
}

// Backward compatibility
void matrix_on(uint8_t x, uint8_t y) { matrix_set(x, y, 1); }
void matrix_off(uint8_t x, uint8_t y) { matrix_set(x, y, 0); }

// Clear all LEDs - only clears buffer
void matrix_clear(void)
{
    for (int i = 0; i < 8; i++) {
        display_buffer[i] = 0;
    }
}
