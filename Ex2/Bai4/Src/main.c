#include "gpio.h"
#include "timer.h"

int main(void)
{
    GPIO_Init();
    TIM2_Init();

    while (1)
    {
    }
}
