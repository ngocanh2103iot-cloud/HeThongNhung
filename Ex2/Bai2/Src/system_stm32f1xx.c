#include "stm32f1xx.h"

uint32_t SystemCoreClock = 72000000;

void SystemInit(void)
{
    FLASH->ACR = (1 << 4) | 2;

    RCC->CR |= (1 << 16);
    while ((RCC->CR & (1 << 17)) == 0) {
    }

    RCC->CFGR &= ~((0x3 << 0) | (0xF << 4) | (0x7 << 8) |
                   (0x7 << 11) | (1 << 16) | (1 << 17) |
                   (0xF << 18));
    RCC->CFGR |= (0x4 << 8) | (1 << 16) | (0x7 << 18);

    RCC->CR |= (1 << 24);
    while ((RCC->CR & (1 << 25)) == 0) {
    }

    RCC->CFGR &= ~(0x3 << 0);
    RCC->CFGR |= (0x2 << 0);
    while ((RCC->CFGR & (0x3 << 2)) != (0x2 << 2)) {
    }

    SystemCoreClock = 72000000;
}

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = 72000000;
}

/* The selected CMSIS startup calls this before main.  This C-only project has
 * no static constructors and links without a C runtime library.
 */
void __libc_init_array(void)
{
}
