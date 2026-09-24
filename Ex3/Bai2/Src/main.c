#include "stm32f1xx.h"
#include "max7219.h"

// Tham số chuyển động
#define ANIM_DELAY_MS  200
#define TEST_MODE      1    // 0: chuyển động, 1: mẫu thử

// Hình tim 6 hàng x 8 cột; 1 là LED bật
static const uint8_t heart[] = {
    0b01100110,  // .##..##.
    0b11111111,  // ########
    0b11111111,  // ########
    0b01111110,  // .######.
    0b00111100,  // ..####..
    0b00011000   // ...##...
};

// Kích thước hình tim
#define HEART_HEIGHT  6
#define HEART_WIDTH   8

// Trạng thái chuyển động
static int8_t offset_y = 0;       // Độ lệch dọc
static int8_t direction = 1;      // 1: xuống, -1: lên
static volatile uint32_t tick_counter = 0;

// Chỉ số mẫu thử
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

    // Nháy LED
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

// Vẽ tim theo độ lệch dọc
static void draw_heart(int8_t offset)
{
    matrix_clear();

    // Vẽ từng hàng
    for (uint8_t row = 0; row < HEART_HEIGHT; row++) {
        uint8_t dest_y = row + offset;

        // Bỏ hàng ngoài màn hình
        if (dest_y > 7) continue;

        uint8_t bits = heart[row];

        // Vẽ từng cột
        for (uint8_t col = 0; col < HEART_WIDTH; col++) {
            matrix_set(col, dest_y, (bits >> (7 - col)) & 1);
        }
    }

    matrix_update();
}

// Mẫu thử ánh xạ hàng
static void run_test_pattern(void)
{
    // Mẫu 0: bật hàng 0
    if (test_pattern == 0) {
        matrix_clear();
        for (uint8_t x = 0; x < 8; x++) {
            matrix_set(x, 0, 1);
        }
        matrix_update();
    }
    // Mẫu 1: bật hàng 7
    else if (test_pattern == 1) {
        matrix_clear();
        for (uint8_t x = 0; x < 8; x++) {
            matrix_set(x, 7, 1);
        }
        matrix_update();
    }
    // Mẫu 2: vẽ "TOP" ở hàng 0-2
    else if (test_pattern == 2) {
        matrix_clear();
        // Chữ T, hàng 0
        matrix_set(0, 0, 1); matrix_set(1, 0, 1); matrix_set(2, 0, 1);
        matrix_set(3, 0, 1); matrix_set(4, 0, 1); matrix_set(5, 0, 1);
        matrix_set(6, 0, 1); matrix_set(7, 0, 1);
        // Chữ O, hàng 1
        matrix_set(0, 1, 1); matrix_set(7, 1, 1);
        // Chữ P, hàng 2
        matrix_set(0, 2, 1); matrix_set(7, 2, 1);
        matrix_update();
    }
    // Mẫu 3: vẽ "BOT" ở hàng 5-7
    else if (test_pattern == 3) {
        matrix_clear();
        // Chữ B, hàng 5
        matrix_set(0, 5, 1); matrix_set(7, 5, 1);
        // Chữ O, hàng 6
        matrix_set(0, 6, 1); matrix_set(7, 6, 1);
        // Chữ T, hàng 7
        matrix_set(0, 7, 1); matrix_set(1, 7, 1); matrix_set(2, 7, 1);
        matrix_set(3, 7, 1); matrix_set(4, 7, 1); matrix_set(5, 7, 1);
        matrix_set(6, 7, 1); matrix_set(7, 7, 1);
        matrix_update();
    }
    // Mẫu 4: tim tĩnh
    else if (test_pattern == 4) {
        draw_heart(1);  // Căn giữa tim
    }
    // Mẫu 5-7: thử độ lệch của tim
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
        // Đổi mẫu mỗi 2 giây
        if (tick_counter - last_test_change >= 2000) {
            last_test_change = tick_counter;
            test_pattern = (test_pattern + 1) % 8;
            run_test_pattern();
        }
#else
        // Chạy chuyển động
        if (tick_counter >= ANIM_DELAY_MS) {
            tick_counter = 0;

            // Cập nhật độ lệch
            offset_y += direction;

            // Đổi hướng tại biên
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
