

#include "DHT11.h"


#define _XTAL_FREQ  8000000

extern BYTE TOUT, CheckSum;
//extern BYTE Temperatura, HR;

void initDHT11(){

    T1CON=0x00;
    T1CONbits.T1CKPS =0;
    TMR1IF =0;
    TMR1IE = 1;
    TMR1=0;
}

//***************************************************************

void StartSignal() {

    DATA_DIR = 0;
    DATA = 0;
    __delay_ms(25);
    DATA = 1;
    __delay_us(30);
    DATA_DIR = 1;


}

//***********************************************************************

BYTE CheckResponse() {


    TOUT = 0;
    TMR1 = 0;
    TMR1ON = 1;

    while (!DATA && !TOUT);

   
    if (TOUT) return 0;
    else {
        
        TMR1 = 0;
        while (DATA && !TOUT);
       
        if (TOUT){
            
            return 0;

        }
        else {
           
            TMR1ON = 0;
            return 1;
        }

    }

}
//**************************************************************************

BYTE readByte(){

    BYTE num = 0, i;
    DATA_DIR = 1;


    for (i=0; i<8; i++){

        while(!DATA);
        TMR1 = 0;
        TMR1ON = 1;
        while(DATA);
        TMR1ON = 0;
        if(TMR1 > 80) num |= 1<<(7-i);


    }

    return num;

}

//**************************************************************************
BOOL mideDHT11(BYTE *T,BYTE *RH)
{
    BYTE check = 0;
    BYTE T_Byte1, T_Byte2, RH_Byte1, RH_Byte2;
    BYTE CheckSum;


    StartSignal();
    check = CheckResponse();

    if (!check) {

       return 0;

    } else {

        RH_Byte1 = readByte();
        RH_Byte2 = readByte();
        T_Byte1 = readByte();
        T_Byte2 = readByte();
        CheckSum = readByte();

        if (CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF))
        {

            *T=T_Byte1;
            *RH=RH_Byte1;
            return 1;

        } else
            return 0;

    }

}