#include "stm32f1xx.h"
#include "max7219.h"

// Tham số chuyển động
#define ANIM_DELAY_MS  200

// Ảnh trái tim 6 hàng, 8 cột
static const uint8_t heart[] = {
    0b01100110,  // .##..##.
    0b11111111,  // ########
    0b11111111,  // ########
    0b01111110,  // .######.
    0b00111100,  // ..####..
    0b00011000   // ...##...
};

#define HEART_HEIGHT  6
#define HEART_WIDTH   8

// Trạng thái chuyển động
static int8_t offset_y = 0;
static int8_t direction = 1;

static volatile uint32_t tick_counter = 0;


/* =========================================================
 * KHỞI TẠO GPIO
 * ========================================================= */
static void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                    GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
                    GPIO_CRL_MODE2 | GPIO_CRL_CNF2);

    GPIOA->CRL |= GPIO_CRL_MODE0_1 |
                  GPIO_CRL_MODE1_1 |
                  GPIO_CRL_MODE2_1;
}


/* =========================================================
 * SYSTICK
 * ========================================================= */
void SysTick_Handler(void)
{
    tick_counter++;

    // Điều khiển LED nhấp nháy
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


/* =========================================================
 * VẼ TRÁI TIM
 * ========================================================= */
static void draw_heart(int8_t offset)
{
    matrix_clear();

    for (uint8_t row = 0; row < HEART_HEIGHT; row++) {

        int8_t dest_y = (int8_t)row + offset;

        // Bỏ qua điểm ngoài ma trận
        if (dest_y < 0 || dest_y > 7) {
            continue;
        }

        uint8_t bits = heart[row];

        for (uint8_t col = 0; col < HEART_WIDTH; col++) {

            uint8_t state =
                (bits >> (7 - col)) & 0x01;

            matrix_set(
                col,
                (uint8_t)dest_y,
                state
            );
        }
    }

    matrix_update();
}


/* =========================================================
 * HÀM CHÍNH
 * ========================================================= */
int main(void)
{
    GPIO_Init();

    // Chu kỳ SysTick 1 ms
    SysTick_Config(SystemCoreClock / 1000U);

    // Khởi tạo MAX7219
    max7219_init();

    // Hiển thị trái tim ban đầu
    draw_heart(offset_y);

    while (1)
    {
        if (tick_counter >= ANIM_DELAY_MS)
        {
            tick_counter = 0;

            // Di chuyển trái tim
            offset_y += direction;

            // Chạm đáy
            if (offset_y >= 2)
            {
                offset_y = 2;
                direction = -1;
            }

            // Chạm đỉnh
            else if (offset_y <= 0)
            {
                offset_y = 0;
                direction = 1;
            }

            draw_heart(offset_y);
        }
    }
}
