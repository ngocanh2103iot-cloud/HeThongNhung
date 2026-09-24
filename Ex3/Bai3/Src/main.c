/* ==================== PHAN 1: THU VIEN ==================== */

#include "stm32f1xx.h"

#include <stdint.h>

/* ==================== PHAN 2: BIEN DUNG CHUNG ==================== */

#define UART_BAUD_RATE       9600U
#define BUTTON_RELEASE_MS    20U
#define UART_TX_BUFFER_SIZE  32U

/* Thay hai ID nay theo thong tin cua lop va nhom. */
#define CLASS_ID  "ELE1415_02_"
#define GROUP_ID  "03 "

static char uart_tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint32_t button_count;
static volatile uint8_t uart_dma_busy;
static volatile uint8_t button_is_pressed;

/* ==================== PHAN 3: KHOI TAO NGOAI VI ==================== */

static void Gpio_Init(void)
{
    /* APB2ENR: bit 2 IOPAEN, bit 4 IOPCEN, bit 0 AFIOEN. */
    RCC->APB2ENR |= (1 << 2) | (1 << 4) | (1 << 0);

    /* PA9: USART1_TX, chuc nang thay the day-keo 50 MHz. */
    GPIOA->CRH = (GPIOA->CRH & ~(0xF << 4)) | (0xB << 4);

    /* PC13: ngo vao keo len; nut nhan noi xuong GND. */
    GPIOC->CRH = (GPIOC->CRH & ~(0xF << 20)) | (0x8 << 20);
    GPIOC->BSRR = (1 << 13);
}

static void Uart_Init(void)
{
    /* APB2ENR bit 14: USART1EN. */
    RCC->APB2ENR |= (1 << 14);

    /* PCLK2 = 72 MHz, 8 bit du lieu, khong chan le, 1 bit dung. */
    USART1->BRR = (SystemCoreClock + (UART_BAUD_RATE / 2U)) / UART_BAUD_RATE;
    USART1->CR2 = 0U;
    USART1->CR3 = (1 << 7);             /* DMAT */
    USART1->CR1 = (1 << 3) | (1 << 13); /* TE | UE */
}

static void Dma_Init(void)
{
    RCC->AHBENR |= (1 << 0); /* DMA1EN */

    DMA1_Channel4->CCR = 0U;
    DMA1_Channel4->CPAR = (uint32_t)(uintptr_t)&USART1->DR;
    DMA1_Channel4->CCR = (1 << 4) |  /* DIR: bo nho sang ngoai vi */
                         (1 << 7) |  /* MINC */
                         (1 << 1) |  /* TCIE */
                         (1 << 12);  /* Uu tien trung binh */
    DMA1->IFCR = (1 << 12);          /* CGIF4 */

    NVIC_SetPriority(DMA1_Channel4_IRQn, 1U);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

static void Button_Init(void)
{
    /* EXTICR4[7:4] = 0010: noi EXTI13 voi cong C. */
    AFIO->EXTICR[3] = (AFIO->EXTICR[3] & ~(0xF << 4)) |
                       (0x2 << 4);

    EXTI->IMR |= (1 << 13);   /* Bo chan ngat duong 13. */
    EXTI->RTSR &= ~(1 << 13); /* Khong bat canh len. */
    EXTI->FTSR |= (1 << 13);  /* Bat canh xuong. */
    EXTI->PR = (1 << 13);     /* Xoa co ngat dang cho. */

    NVIC_SetPriority(EXTI15_10_IRQn, 2U);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* ==================== PHAN 4: GUI DU LIEU ==================== */

static uint16_t Format_Button_Message(uint32_t count)
{
    static const char prefix[] = CLASS_ID GROUP_ID ":BTN:";
    char reversed_digits[10];
    uint16_t length = 0U;
    uint8_t digit_count = 0U;

    for (uint8_t i = 0U; i < (uint8_t)(sizeof(prefix) - 1U); ++i) {
        uart_tx_buffer[length++] = prefix[i];
    }

    do {
        reversed_digits[digit_count++] = (char)('0' + (count % 10U));
        count /= 10U;
    } while (count != 0U);

    while (digit_count != 0U) {
        uart_tx_buffer[length++] = reversed_digits[--digit_count];
    }

    /* Dung dung thu tu ky tu ket thuc theo de bai: \n\r. */
    uart_tx_buffer[length++] = '\n';
    uart_tx_buffer[length++] = '\r';

    return length;
}

static void Send_Buffer_DMA(uint16_t length)
{
    DMA1_Channel4->CCR &= ~(1 << 0); /* EN = 0 */
    DMA1->IFCR = (1 << 12);          /* CGIF4 */
    DMA1_Channel4->CMAR = (uint32_t)(uintptr_t)uart_tx_buffer;
    DMA1_Channel4->CNDTR = length;
    uart_dma_busy = 1U;
    __DMB();
    DMA1_Channel4->CCR |= (1 << 0); /* EN = 1 */
}

/* ==================== PHAN 5: HAM NGAT ==================== */

void SysTick_Handler(void)
{
    static uint8_t release_stable_ms;

    if (button_is_pressed == 0U) {
        release_stable_ms = 0U;
        return;
    }

    if ((GPIOC->IDR & (1 << 13)) != 0U) {
        if (++release_stable_ms >= BUTTON_RELEASE_MS) {
            button_is_pressed = 0U;
            release_stable_ms = 0U;
        }
    } else {
        release_stable_ms = 0U;
    }
}

void EXTI15_10_IRQHandler(void)
{
    if ((EXTI->PR & (1 << 13)) != 0U) {
        EXTI->PR = (1 << 13);

        if (((GPIOC->IDR & (1 << 13)) == 0U) &&
            (button_is_pressed == 0U) &&
            (uart_dma_busy == 0U)) {
            uint16_t message_length;

            button_is_pressed = 1U;
            ++button_count;
            message_length = Format_Button_Message(button_count);
            Send_Buffer_DMA(message_length);
        }
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if ((DMA1->ISR & (1 << 13)) != 0U) { /* TCIF4 */
        DMA1_Channel4->CCR &= ~(1 << 0); /* EN = 0 */
        DMA1->IFCR = (1 << 12);          /* CGIF4 */
        uart_dma_busy = 0U;
    }
}

/* ==================== PHAN 6: HAM MAIN ==================== */

int main(void)
{
    Gpio_Init();
    Uart_Init();
    Dma_Init();
    Button_Init();

    (void)SysTick_Config(SystemCoreClock / 1000U);

    while (1) {
        __WFI();
    }
}
