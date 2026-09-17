#include "uart_init.h"

int main(void)
{
    UART_Init();

    while (1) {
        UART_Process();
    }
}
