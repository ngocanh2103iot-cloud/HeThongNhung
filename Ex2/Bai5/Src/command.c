#include "command.h"
#include "pwm_init.h"
#include "uart_init.h"

// Chi dung de gui phan tram 0..100, khong can printf.
void Send_Number(int number){
    if(number >= 100) Send_Char('0' + number / 100);
    if(number >= 10) Send_Char('0' + (number / 10) % 10);
    Send_Char('0' + number % 10);
}

int Compare_Command(const char *string){
    int j = 0;
    while(string[j] != '\0' && buffer[j] == string[j]) j++;
    return string[j] == '\0' && buffer[j] == '\0';
}

void Send_Status(void){
    if(led_on) Send_String("LED=ON; PWM=");
    else Send_String("LED=OFF; PWM=");
    Send_Number(pwm_percent);
    Send_String("%; OUTPUT=");
    Send_Number(led_on ? pwm_percent : 0);
    Send_String("%\r\n");
}

void Process_Command(void){
    int j;
    int value;
    int digits;

    if(!ready) return;

    if(rx_error){
        Send_String("ERROR: RX or command too long\r\n");
        goto finish;
    }
    if(Compare_Command("ON")){
        led_on = 1;
    }
    else if(Compare_Command("OFF")){
        led_on = 0;
    }
    else if(Compare_Command("Status")){
        Send_Status();
        goto finish;
    }
    else if(buffer[0] == 'P' && buffer[1] == 'W' &&
            buffer[2] == 'M' && buffer[3] == ':'){
        j = 4;
        value = 0;
        digits = 0;
        while(buffer[j] >= '0' && buffer[j] <= '9'){
            digits++;
            if(digits > 3) break;
            value = value * 10 + buffer[j] - '0';
            j++;
        }
        if(digits == 0 || digits > 3 || value > 100 ||
           buffer[j] != '%' || buffer[j + 1] != '\0'){
            Send_String("ERROR: use PWM:0%!..PWM:100%!\r\n");
            goto finish;
        }
        pwm_percent = value;
    }
    else{
        Send_String("ERROR: use ON! OFF! PWM:50%! Status!\r\n");
        goto finish;
    }
    Led_Update();
    Send_Status();

finish:
    i = 0;
    rx_error = 0;
    ready = 0;
}
