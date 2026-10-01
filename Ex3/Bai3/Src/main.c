#include "stm32f1xx.h" 
#include "uart.h" 
#include "delay.h" 
#include "dma_uart.h" 
#include "format.h" 
/* Cau hinh PC13 lam ngo vao keo len theo code hien tai. */
void Button_Init(){ 
    /* Cap clock GPIOC, chon PC13 input pull-up; muc thap duoc xem la nhan. */
    RCC->APB2ENR |= (1 << 4); 
    GPIOC->CRH &= ~(0xF << 20); 
    GPIOC->CRH |= (0x8 << 20); 
    GPIOC->ODR |= (1 << 13); 
}
/* Phat hien chuyen tu muc 1 sang 0. */
uint8_t Button_Triggered() 
{
    static uint8_t last_state = 1; 
    uint8_t current_state; 
    /* Doc PC13 va so voi trang thai truoc de chi nhan canh xuong. */
    current_state = (GPIOC->IDR & (1 << 13)) ? 1 : 0; 
    if ((last_state == 1) && (current_state == 0)) 
    {
        /* Cho 20 ms roi doc lai de loc doi khi nhan. */
        Delay_ms(20); 
        current_state = (GPIOC->IDR & (1 << 13)) ? 1 : 0; 
        /* Sau thoi gian loc doi, neu van thap thi ghi nho da nhan va bao mot su kien. */
        if (current_state == 0) 
        {
            last_state = 0; 
            return 1; 
        }
    }
    /* Tha nut thi cho phep nhan lan tiep theo. */
    if (current_state == 1) 
    {
        last_state = 1; 
    }
    return 0; 
}
int main(){ 
    /* Khoi tao UART, tre, DMA va dau vao nut nhan. */
    UART_Init(); 
    Delay_Init(); 
    DMA_UART_Init(); 
    Button_Init(); 
    /* So dem nut nhan va bo dem tinh dung cho DMA. */
    uint8_t counter = 0; 
    static uint8_t tx_buffer[32]; 
    while(1){ 
       if (Button_Triggered()){ 
            /* Tang so dem moi lan phat hien nhan. */
            counter++; 
             /* Chi ghi bo dem khi DMA da san sang. */
             if (dma_done){ 
                /* Ghep ID, so dem va ky tu xuong dong. */
                size_t len = Format_U32((char *)tx_buffer,  
                                        sizeof(tx_buffer), 
                                        "<ELE1415><02>:BTN:",  
                                        counter, "\n\r"); 
                /* Chi gui chuoi hop le; cong 1 de phu hop quy uoc len - 1 cua module DMA. */
                if (len > 0){ 
                    /* Cong 1 vi module DMA hien gui len - 1 byte. */
                    DMA_UART_SendData(tx_buffer, (uint8_t)(len + 1)); 
                }
                }        
            }
        }
}
