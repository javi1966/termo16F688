#include <16F689.h>
#device adc=8
#FUSES NOWDT, INTRC_IO, NOPROTECT, BROWNOUT, NOMCLR, NOCPD, PUT, IESO, FCMEN
#use delay(clock=8000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)

