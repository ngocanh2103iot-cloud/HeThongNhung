#include "stm32f1xx.h"
#include "max7219.h"

// Animation parameters
#define ANIM_DELAY_MS  200
#define TEST_MODE      1    // 0=animation, 1=test patterns

// Heart bitmap (6 rows x 8 cols) - 1 = LED on
static const uint8_t heart[] = {
    0b01100110,  // .##..##.
    0b11111111,  // ########
    0b11111111,  // ########
    0b01111110,  // .######.
    0b00111100,  // ..####..
    0b00011000   // ...##...
};

// Heart dimensions
#define HEART_HEIGHT  6
#define HEART_WIDTH   8

// Animation state
static int8_t offset_y = 0;       // Vertical offset
static int8_t direction = 1;      // 1 = down, -1 = up
static volatile uint32_t tick_counter = 0;

// Test pattern counter
static uint8_t test_pattern = 0;

static void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                    GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
                    GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
    GPIOA->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1;
}

void SysTick_Handler(void)
{
    tick_counter++;

    // Blinking LEDs (existing code)
    static uint16_t led_01hz_ticks;
    static uint16_t led_1hz_ticks;
    static uint16_t led_10hz_ticks;

    if (++led_01hz_ticks == 5000U) {
        led_01hz_ticks = 0U;
        GPIOA->ODR ^= GPIO_ODR_ODR0;
    }

    if (++led_1hz_ticks == 500U) {
        led_1hz_ticks = 0U;
        GPIOA->ODR ^= GPIO_ODR_ODR1;
    }

    if (++led_10hz_ticks == 50U) {
        led_10hz_ticks = 0U;
        GPIOA->ODR ^= GPIO_ODR_ODR2;
    }
}

// Draw heart at vertical offset
static void draw_heart(int8_t offset)
{
    matrix_clear();

    // Draw each row of heart bitmap
    for (uint8_t row = 0; row < HEART_HEIGHT; row++) {
        uint8_t dest_y = row + offset;

        // Boundary check
        if (dest_y > 7) continue;

        uint8_t bits = heart[row];

        // Draw each column
        for (uint8_t col = 0; col < HEART_WIDTH; col++) {
            matrix_set(col, dest_y, (bits >> (7 - col)) & 1);
        }
    }

    matrix_update();
}

// Test patterns to determine row mapping
static void run_test_pattern(void)
{
    // Pattern 0: Fill row 0 only
    if (test_pattern == 0) {
        matrix_clear();
        for (uint8_t x = 0; x < 8; x++) {
            matrix_set(x, 0, 1);
        }
        matrix_update();
    }
    // Pattern 1: Fill row 7 only
    else if (test_pattern == 1) {
        matrix_clear();
        for (uint8_t x = 0; x < 8; x++) {
            matrix_set(x, 7, 1);
        }
        matrix_update();
    }
    // Pattern 2: Draw "TOP" text (row 0-2) to verify Y mapping
    else if (test_pattern == 2) {
        matrix_clear();
        // T row 0
        matrix_set(0, 0, 1); matrix_set(1, 0, 1); matrix_set(2, 0, 1);
        matrix_set(3, 0, 1); matrix_set(4, 0, 1); matrix_set(5, 0, 1);
        matrix_set(6, 0, 1); matrix_set(7, 0, 1);
        // O row 1
        matrix_set(0, 1, 1); matrix_set(7, 1, 1);
        // P row 2
        matrix_set(0, 2, 1); matrix_set(7, 2, 1);
        matrix_update();
    }
    // Pattern 3: Draw "BOT" text (row 5-7) to verify Y mapping
    else if (test_pattern == 3) {
        matrix_clear();
        // B row 5
        matrix_set(0, 5, 1); matrix_set(7, 5, 1);
        // O row 6
        matrix_set(0, 6, 1); matrix_set(7, 6, 1);
        // T row 7
        matrix_set(0, 7, 1); matrix_set(1, 7, 1); matrix_set(2, 7, 1);
        matrix_set(3, 7, 1); matrix_set(4, 7, 1); matrix_set(5, 7, 1);
        matrix_set(6, 7, 1); matrix_set(7, 7, 1);
        matrix_update();
    }
    // Pattern 4: Full heart static
    else if (test_pattern == 4) {
        draw_heart(1);  // Center the heart
    }
    // Pattern 5-7: Heart with offset for animation test
    else if (test_pattern == 5) {
        draw_heart(0);
    }
    else if (test_pattern == 6) {
        draw_heart(1);
    }
    else if (test_pattern == 7) {
        draw_heart(2);
    }
}

int main(void)
{
    GPIO_Init();
    SysTick_Config(SystemCoreClock / 1000U);

    max7219_init();

    uint32_t last_test_change = 0;

    while (1) {
#if TEST_MODE
        // Change test pattern every 2 seconds
        if (tick_counter - last_test_change >= 2000) {
            last_test_change = tick_counter;
            test_pattern = (test_pattern + 1) % 8;
            run_test_pattern();
        }
#else
        // Original animation code
        if (tick_counter >= ANIM_DELAY_MS) {
            tick_counter = 0;

            // Update offset
            offset_y += direction;

            // Bounce at boundaries
            if (offset_y >= 2) {
                offset_y = 2;
                direction = -1;
            } else if (offset_y <= 0) {
                offset_y = 0;
                direction = 1;
            }

            draw_heart(offset_y);
        }
#endif
    }
}
