/* 
 * File:   main.c
 * Author: Javier
 *
 * Created on 8 de enero de 2014, 18:41
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "GenericTypeDefs.h"
#include "DHT11.h"


#pragma config FOSC=INTOSCIO,BOREN=ON,CPD=OFF,MCLRE=OFF,\
               WDTE=OFF,CP=OFF,PWRTE=ON

#define _XTAL_FREQ       8000000

#define LED_T            PORTAbits.RA5
#define TRIS_LED_T       TRISAbits.TRISA5
#define LED_RH           PORTAbits.RA4
#define TRIS_LED_RH      TRISAbits.TRISA4
#define LE1              PORTCbits.RC4
#define LE2              PORTCbits.RC5
#define PIN_T            5
#define PIN_RH           4

#define delay_ms(ms)     {int i;for(i=0;i<ms;i++) __delay_ms(1);}
#define delay_us(us)     {int i;for(i=0;i<us;i++) __delay_us(1);}
#define  PWM(iDuty,pin)   {                         \
     int lDuty = 0;                                  \
     lDuty = ((iDuty) * 100) / 100;                  \
     PORTAbits.RA##pin = 0;                        \
     delay_us(lDuty);                                \
     PORTAbits.RA##pin = 1;                        \
     delay_us(100 - lDuty);                          \
}


//****************** VARIABLES GLOBALES ***************************


extern BYTE TOUT = 0;
BOOL bTemp5Seg = 0;
BOOL bLectura1min = 0;
BOOL bOK = 0;
BYTE Temperatura, HR;

//************** FUNCIONES ****************************

BYTE bin2bcd(BYTE bin) {

    BYTE bcd;

    bcd = (bin / 10) << 4;
    bcd += (bin % 10);

    return bcd;

}
//************************************************************

void visuHP7300(BYTE Num) {

    PORTC = ((bin2bcd(Num) & 0xF0) >> 4) | 0xF0;
    LE1 = 0;
    LE1 = 1;

    PORTC = (bin2bcd(Num) & 0x0F) | 0xF0;
    LE2 = 0;
    LE2 = 1;

}

//^*********************************************************************

int main(int argc, char** argv) {

   
    int j;


    OSCCON = 0b01110001; // 8Mhz

    ANSEL = 0x00; //todo digital
    CMCON0 = 0b00000111;
    TRISA = 0x00;
    TRISC = 0x00;
    TRIS_LED_T = 0;
    LED_T = 1;
    TRIS_LED_RH = 0;
    LED_RH = 1;


    //Configura timer0 para tempos.
    OPTION_REGbits.PSA = 0; //prescaler a TMR0
    OPTION_REGbits.PS = 0b111; //FOSC/4/256
    OPTION_REGbits.T0CS = 0; //FOSC/4   instruccion cicle
    INTCONbits.TMR0IE = 1; //interrupt 255 * 255 * 0.5us aprox 33ms
    INTCONbits.TMR0IF = 0;
    TMR0 = 0;

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;


    Temperatura = 0;
    HR = 0;


    for (j = 0; j < 10; j++) {
        LED_T = 1;
        LED_RH = 0;
        LE1 = 0;
        LE2 = 1;
        __delay_ms(300);
        LED_T = 0;
        LED_RH = 1;
        LE1 = 1;
        LE2 = 0;
        __delay_ms(300);

    }

    initDHT11();

    __delay_ms(300);
    
    bOK = mideDHT11((BYTE*) & Temperatura, (BYTE*) & HR);
    if (!bOK) {
        LED_T = 0;
        LED_RH = 0;

    }


    while (TRUE) {

        if (bLectura1min) {

            bOK = mideDHT11((BYTE*) & Temperatura, (BYTE*) & HR);
            if (!bOK) {
                LED_T = 0;
                LED_RH = 0;

            }
            bLectura1min = 0;
        }

        if (!bTemp5Seg) {
            visuHP7300(Temperatura);
           // LED_T = 0;
            PWM(Temperatura+10,5);   //ajusta brillo
            LED_RH = 1;
        } else {
            visuHP7300(HR);
            LED_T = 1;
            //  LED_RH = 0;
            PWM(HR-20,4);  //ajusta brillo
        }

    }//while
    return (EXIT_SUCCESS);
}

//********* Interrupciones *********************************************

void interrupt isr(void) {


    static int cnt = 0;
    static WORD cnt1min = 0;


    if (TMR1IF) {
        TOUT = 1;
        TMR1ON = 0;
        TMR1IF = 0;

    }

    if (TMR0IF) {

        if (++cnt > 165) { //aprox 5 seg
            bTemp5Seg = !bTemp5Seg;
            cnt = 0;
        }
        if (++cnt1min > 1265) { //1 minuto
            bLectura1min = 1;
            cnt1min = 0;
        }

        INTCONbits.TMR0IF = 0;
    }

}
