#include <16F689.h>
#device adc=8
#FUSES NOWDT, INTRC_IO, NOPROTECT, BROWNOUT, NOMCLR, NOCPD, PUT, IESO, FCMEN
#use delay(clock=8000000)
#use rs232(baud=9600,parity=N,xmit=PIN_B7,rcv=PIN_B5,bits=8)


//**********************************************************************
int1 bInt100m;
int1 bUpDw;




//**************Interrupciones******************************************************
#INT_RTCC
void intTareas()
{
 
   static int iTemp=0;   //
  

  if(++iTemp>12)          //cuenta cada aprox. 100ms,cambia estado cada seg.
  {
   bInt100m=TRUE;
   iTemp=0;
  }

  

}


//********************************************************************************
void PWM(int8 iDuty)
{
  unsigned long lDuty=0L;
  int16 lY;
  
     
 
      lDuty=(iDuty*1000)/100;    
      //printf(",%lu",lDuty);
      output_low(PIN_A0);
      delay_us(lDuty);
      output_high(PIN_A0);
      lY=1000-lDuty;
      //printf(",%lu",lY);
      delay_us(lY);//1000-lDuty);
}
//*********************************************************************************

void main()
{

  unsigned int iNivel;

   setup_adc_ports(NO_ANALOGS|VSS_VDD);
   setup_adc(ADC_OFF);
   setup_spi(SPI_SS_DISABLED);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   setup_comparator(NC_NC_NC_NC);
   
   setup_oscillator(OSC_8MHZ|OSC_INTRC); 
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_64);  //
   
   
   bUpDw=TRUE;
   bInt100m=FALSE;
   iNivel=1;
   
   printf("\r\nHola");
   
   output_low(PIN_C0);
   output_high(PIN_C1);
   
   
   output_high(PIN_C0);
   output_low(PIN_C1);
   
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);
 
   while(TRUE)
   {
     if(bInt100m)  //cada 100ms
     {
       //printf("\r\n%d,%d",bUpDw,iNivel);
      if(bUpDw)
      {
        
        if(++iNivel > 99)
        {
         iNivel=99;
         bUpDw=FALSE;
        }
      }
      else
      {
        
        if(--iNivel ==0xFF)  //¿¿ Roll-over ??
        {
          iNivel=1;
          bUpDw=TRUE;
        }  
      }  
        
        
        bInt100m=FALSE;
     }
     
    PWM(iNivel);
   }
}
