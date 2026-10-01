#include "stm32f1xx.h" 
#include "delay.h" 

/* Cau hinh SysTick cho moi chu ky 1 ms, chua bat dem. */
void Delay_Init(void) 
{
    /* Clock CPU/8; 72 MHz / 8 / 1000 = 9000 nhip moi ms. */
    SysTick->CTRL = 0; 
    SysTick->LOAD = SystemCoreClock / 8U / 1000U - 1U;
    SysTick->VAL = 0; 
}

/* Cho chan theo ms bang polling; dung rieng SysTick, khong dung ngat. */
void Delay_ms(uint32_t ms) 
{
    /* Tra ve ngay khi khong can tre, khong bat bo dem. */
    if (ms == 0U) 
    {
        return; 
    }

    /* Xoa gia tri va co cu, bat dem khong dung ngat. */
    SysTick->VAL = 0; 
    SysTick->CTRL = (1 << 0); 

    /* Moi lan COUNTFLAG bat tinh la mot ms; doc CTRL de xoa co va tiep tuc dem. */
    while (ms > 0U) 
    {
        /* Cho COUNTFLAG (bit 16) bao het 1 ms. */
        while ((SysTick->CTRL & (1 << 16)) == 0) 
        {
        }
        ms--; 
    }

    SysTick->CTRL = 0; 
}
