#include "stm32f1xx.h"

#define ADC_REFERENCE_MV 3300U
#define ADC_MAX_VALUE    4095U

static volatile uint32_t milliseconds;

void SysTick_Handler(void)
{
    ++milliseconds;
}

static void ADC_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE) | RCC_CFGR_ADCPRE_DIV6;

    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    ADC1->SMPR2 = (ADC1->SMPR2 & ~ADC_SMPR2_SMP0) |
                  ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_0;
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->CR2 = ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL | ADC_CR2_ADON;

    for (uint32_t delay = 0U; delay < 100U; ++delay) {
        __NOP();
    }

    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) != 0U) {
    }

    ADC1->CR2 |= ADC_CR2_CAL;
    while ((ADC1->CR2 & ADC_CR2_CAL) != 0U) {
    }
}

static uint16_t ADC_Read(void)
{
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0U) {
    }

    return (uint16_t)ADC1->DR;
}

static void UART_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1;

    USART1->BRR = 7500U;
    USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
}

static void UART_SendChar(char character)
{
    while ((USART1->SR & USART_SR_TXE) == 0U) {
    }
    USART1->DR = (uint16_t)character;
}

static void UART_SendString(const char *string)
{
    while (*string != '\0') {
        UART_SendChar(*string++);
    }
}

static void UART_SendVoltage(uint16_t adc_value)
{
    uint32_t millivolts = ((uint32_t)adc_value * ADC_REFERENCE_MV) /
                          ADC_MAX_VALUE;

    UART_SendString("Voltage: ");
    UART_SendChar((char)('0' + millivolts / 1000U));
    UART_SendChar('.');
    UART_SendChar((char)('0' + (millivolts / 100U) % 10U));
    UART_SendChar((char)('0' + (millivolts / 10U) % 10U));
    UART_SendChar((char)('0' + millivolts % 10U));
    UART_SendString(" V\r\n");
}

int main(void)
{
    ADC_Init();
    UART_Init();
    SysTick_Config(SystemCoreClock / 1000U);

    uint32_t last_report = milliseconds;

    while (1) {
        if ((milliseconds - last_report) >= 1000U) {
            last_report += 1000U;
            UART_SendVoltage(ADC_Read());
        }
    }
}
