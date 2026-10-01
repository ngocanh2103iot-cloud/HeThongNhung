#include "stm32f1xx.h"
#include "uart.h"
#include "delay.h"
#include "dma_uart.h"
#include "format.h"
void Button_Init(){
    RCC->APB2ENR |= (1 << 4);
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x8 << 20);
    GPIOC->ODR |= (1 << 13);
}
uint8_t Button_Triggered()
{
    static uint8_t last_state = 1;
    uint8_t current_state;
    current_state = (GPIOC->IDR & (1 << 13)) ? 1 : 0;
    if ((last_state == 1) && (current_state == 0))
    {
        Delay_ms(20);
        current_state = (GPIOC->IDR & (1 << 13)) ? 1 : 0;
        if (current_state == 0)
        {
            last_state = 0;
            return 1;
        }
    }
    if (current_state == 1)
    {
        last_state = 1;
    }
    return 0;
}
int main(){
    Uart_Init();
    Delay_Init();
    Dma_Uart_Init();
    Button_Init();
    uint8_t counter = 0;
    static uint8_t tx_buffer[32];
    while(1){
       if (Button_Triggered()){
            counter++;
             if (dma_done){
                size_t len = Format_U32((char *)tx_buffer, 
                                        sizeof(tx_buffer),
                                        "<ELE1415><02>:BTN:", 
                                        counter, "\n\r");
                if (len > 0){
                    Dma_Uart_SendData(tx_buffer, (uint8_t)(len + 1));
                }
                }        
            }
        }
}
