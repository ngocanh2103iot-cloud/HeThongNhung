#include "stm32f1xx.h"

#define UART_BAUD_RATE          9600U
#define ADC_SAMPLE_RATE_HZ      100U
#define ADC_BUFFER_SIZE         100U
#define ADC_HALF_BUFFER_SIZE    (ADC_BUFFER_SIZE / 2U)
#define ADC_REFERENCE_MV        3300U
#define ADC_MAX_VALUE           4095U

static volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
static volatile uint8_t first_half_ready;
static volatile uint8_t second_half_ready;

static void Uart_Init(void)
{
    RCC->APB2ENR |= (1 << 2) | (1 << 14);

    /* PA9: USART1_TX, chuc nang thay the day-keo 50 MHz. */
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |= (0xB << 4);

    /* PCLK2 = 72 MHz, 8 bit du lieu, khong chan le, 1 bit dung. */
    USART1->BRR = (SystemCoreClock + (UART_BAUD_RATE / 2U)) /
                  UART_BAUD_RATE;
    USART1->CR1 = (1 << 3) | (1 << 13);
}

static void Uart_Send_Char(char c)
{
    while ((USART1->SR & (1 << 7)) == 0U)
    {
    }

    USART1->DR = (uint8_t)c;
}

static void Uart_Send_U32(uint32_t value)
{
    char buffer[10];
    uint8_t i = 0U;

    if (value == 0U)
    {
        Uart_Send_Char('0');
        return;
    }

    while (value > 0U)
    {
        buffer[i] = (char)('0' + (value % 10U));
        value /= 10U;
        i++;
    }

    while (i > 0U)
    {
        i--;
        Uart_Send_Char(buffer[i]);
    }
}

static void Uart_Send_Voltage(uint32_t voltage_mv)
{
    Uart_Send_U32(voltage_mv / 1000U);
    Uart_Send_Char('.');
    Uart_Send_Char((char)('0' + ((voltage_mv / 100U) % 10U)));
    Uart_Send_Char((char)('0' + ((voltage_mv / 10U) % 10U)));
    Uart_Send_Char((char)('0' + (voltage_mv % 10U)));
    Uart_Send_Char(' ');
    Uart_Send_Char('V');
}

static void Uart_Send_Samples(uint16_t start, uint16_t count)
{
    uint16_t i;
    uint32_t voltage_mv;

    for (i = 0U; i < count; i++)
    {
        /* Quy doi ADC 12 bit sang mV voi VREF+ = 3,3 V. */
        voltage_mv = ((uint32_t)adc_buffer[start + i] * ADC_REFERENCE_MV) /
                     ADC_MAX_VALUE;
        Uart_Send_Voltage(voltage_mv);
        Uart_Send_Char('\n');
        Uart_Send_Char('\r');
    }
}

static void Adc_Init(void)
{
    volatile uint32_t delay;

    RCC->APB2ENR |= (1 << 2) | (1 << 9);

    /* PCLK2 = 72 MHz, xung ADC = 72 / 6 = 12 MHz. */
    RCC->CFGR &= ~(3 << 14);
    RCC->CFGR |= (2 << 14);

    /* PA0: ngo vao tuong tu ADC1_IN0. */
    GPIOA->CRL &= ~(0xF << 0);

    ADC1->CR1 = 0U;
    ADC1->CR2 = 0U;

    /* Kenh 0, lay mau 239.5 chu ky. */
    ADC1->SQR1 &= ~(0xF << 20);
    ADC1->SQR3 &= ~(0x1F << 0);
    ADC1->SMPR2 &= ~(7 << 0);
    ADC1->SMPR2 |= (7 << 0);

    /* DMA, kich hoat boi TIM3_TRGO, canh len. */
    ADC1->CR2 |= (1 << 8) | (4 << 17) | (1 << 20);

    ADC1->CR2 |= (1 << 0);
    for (delay = 0U; delay < 1000U; delay++)
    {
    }

    ADC1->CR2 |= (1 << 3);
    while ((ADC1->CR2 & (1 << 3)) != 0U)
    {
    }

    ADC1->CR2 |= (1 << 2);
    while ((ADC1->CR2 & (1 << 2)) != 0U)
    {
    }
}

static void Dma_Init(void)
{
    RCC->AHBENR |= (1 << 0);

    DMA1_Channel1->CCR = 0U;
    DMA1_Channel1->CPAR = (uint32_t)(uintptr_t)&ADC1->DR;
    DMA1_Channel1->CMAR = (uint32_t)(uintptr_t)adc_buffer;
    DMA1_Channel1->CNDTR = ADC_BUFFER_SIZE;
    DMA1_Channel1->CCR = (1 << 1) |   /* Ngat truyen xong. */
                         (1 << 2) |   /* Ngat nua bo dem. */
                         (1 << 5) |   /* Che do vong. */
                         (1 << 7) |   /* Tang dia chi bo nho. */
                         (1 << 8) |   /* Ngoai vi 16 bit. */
                         (1 << 10) |  /* Bo nho 16 bit. */
                         (2 << 12);   /* Uu tien cao. */

    DMA1->IFCR = (1 << 0);
    NVIC_SetPriority(DMA1_Channel1_IRQn, 1U);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    DMA1_Channel1->CCR |= (1 << 0);
}

static void Timer_Init(void)
{
    RCC->APB1ENR |= (1 << 1);

    TIM3->CR1 = 0U;
    TIM3->CR2 = 0U;

    /* Xung TIM3 72 MHz: 72 MHz / 72 / 10000 = 100 Hz. */
    TIM3->PSC = 71U;
    TIM3->ARR = (1000000U / ADC_SAMPLE_RATE_HZ) - 1U;

    /* Nap PSC va ARR truoc khi dua su kien cap nhat ra TRGO. */
    TIM3->EGR = (1 << 0);
    TIM3->SR = 0U;

    TIM3->CR2 = (2 << 4); /* Su kien cap nhat la TRGO. */
    TIM3->CR1 |= (1 << 0);
}

void DMA1_Channel1_IRQHandler(void)
{
    if ((DMA1->ISR & (1 << 2)) != 0U)
    {
        DMA1->IFCR = (1 << 2);
        first_half_ready = 1U;
    }

    if ((DMA1->ISR & (1 << 1)) != 0U)
    {
        DMA1->IFCR = (1 << 1);
        second_half_ready = 1U;
    }
}

int main(void)
{
    Uart_Init();
    Adc_Init();
    Dma_Init();
    Timer_Init();

    while (1)
    {
        if (first_half_ready != 0U)
        {
            first_half_ready = 0U;
            Uart_Send_Samples(0U, ADC_HALF_BUFFER_SIZE);
        }

        if (second_half_ready != 0U)
        {
            second_half_ready = 0U;
            Uart_Send_Samples(ADC_HALF_BUFFER_SIZE,
                              ADC_HALF_BUFFER_SIZE);
        }
    }
}
