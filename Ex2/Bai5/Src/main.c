#include "command.h"
#include "pwm_init.h"
#include "uart_init.h"

int main(void){
    Pwm_Init();
    Uart_Init();
    Send_String("READY: 9600 8N1, end command with !\r\n");

    while(1){
        Process_Command();
    }
}
