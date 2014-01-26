//Termometro con ds18s20 y display HP7300
#include <16F688.h>
#device *=16
#device adc=8
#FUSES NOWDT, INTRC_IO, NOPROTECT, BROWNOUT, NOMCLR, NOCPD, PUT, IESO, FCMEN
#use delay(clock=8000000)
//#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)

#include "ds18s20.c"    //SENSOR TEMPERATURA

#bit  LE1=0x07.4  // display LSB
#bit  LE2=0x07.5

#bit  GRADO=0x05.5   //led con fade indicando grado
#byte PORTC=0x07

 //Nuevos tipos de datos *****************************************************

typedef int1 bool; 

//********* Variables Globales

bool bInt5seg=FALSE;  //tiempo de 5seg. para sample temperatura
bool bInt100m=FALSE;
bool bUpDw=FALSE;


//**************Interrupciones******************************************************
#INT_RTCC
void intTareas()
{
  
   static int iTemp=0;   //cuenta para vis. temperatura
   static int iTemp2=0;

  if(++iTemp>2)          //cuenta cada 100 ms,cambia estado cada seg.
  {
   bInt100m=TRUE;
   iTemp=0;
  }

  if(++iTemp2>1220)   //10 seg.
  {
    bInt5seg=TRUE;
    iTemp2=0;
  }


}
//********************************************************************************
//PWM SW
void PWM(int8 iDuty)
{
  unsigned long lDuty=0L;
  int16 lY;
  
      lDuty=(iDuty*1000)/100;    
      
      output_low(PIN_A5);
      delay_us(lDuty);
      output_high(PIN_A5);
      delay_us(1000-lDuty);
      delay_ms(1);
}

//**********************************************************************
int8 bin2bcd(int8 bin)
{
  int8 bcd;
   
   bcd=(bin/10)<<4;
   bcd+=(bin%10);
   
   return (bcd);
}

//**********************************************************************
void visuHP7300(int8 Num)
{
  int iPrimerDig;
  
        
    PORTC=((bin2bcd(Num) & 0xF0) >> 4) | 0xF0 ;  //nibble inferior PORTC0..3
    LE1=0;LE1=1;
    PORTC=(bin2bcd(Num) & 0x0F) | 0xF0;
    LE2=0;LE2=1;
    
}   

//*****************************************************************************
void beep(int16 tone,int16 playtime)
{
  int j,k; 
  int16 i;
  int cycle; 
  j=tone/255;
  cycle=tone-j*255;
  for(i=0;i<playtime;i++)
  {
    output_low(PIN_A2);
    for(k=0;k<j;k++)
       delay_us(255);
    delay_us(cycle);
    output_high(PIN_A2);
    for(k=0;k<j;k++)
       delay_us(255);
    delay_us(cycle);
  }
}

//**************************************************************************
void main()
{
  int8 iNivel;
  int8 iTemperatura;

  
   setup_adc_ports(NO_ANALOGS|VSS_VDD);
   setup_adc(ADC_OFF);
   
   setup_timer_1(T1_DISABLED);
   setup_comparator(NC_NC);
   setup_vref(FALSE);
   
   setup_oscillator(OSC_8MHZ|OSC_INTRC); 
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_64); // 8192us
   
   
   set_tris_a(0b00010000);
   set_tris_c(0b00000000);
   
  
    bUpDw=TRUE;
    bInt100m=FALSE;
    iNivel=1;
   
   iTemperatura=leeTempDS18B20();
   
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);
   
   delay_ms(1000);
   
   while(TRUE)
   {
     visuHP7300(iTemperatura);
    
   
    if(bInt100m)  //cada 8192us
     {

      if(bUpDw)
      {
       
        if(++iNivel > 40)
        {
         iNivel=40;
         bUpDw=FALSE;
        }
      }
      else
      {
        
        if(--iNivel ==0xFF)  //¿¿ Roll-over ??
        {
          iNivel=0;
          bUpDw=TRUE;
        }  
      }  
      
      
        
       bInt100m=FALSE;
     }
    
     
    if(bInt5seg)     //muestrea sensor DS1820,cada 5 seg.
    {
     iTemperatura=leeTempDS18B20();
     bInt5seg=FALSE;
    }
    
    
    PWM(iNivel);
    
    
   } //While 

}
