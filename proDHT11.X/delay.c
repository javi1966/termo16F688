#include "delay.h"

//Retardo para  FCY=8Mhz/4=0,5 us
// 1us=2*0,5us
//***************************************************

void delay_us(int us) {
    while (us != 0) {
        asm("nop");  //0.5us
        asm("nop");  //0.5us
         us--;
    }
}

//********************************************************

void delay_ms(int ms) {
    while (ms != 0) {
        delay_us(1000);
        ms--;
    }
}


