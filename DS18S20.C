



//***************************************************************

//RUTINAS DEL SENSOR DE TEMPERATURA DS18B20


#define DQ18S20 PIN_A4

//**************************************************************


void initDS18S20()
{
  
  output_low(DQ18S20);   //secuencia segun DATASHEET
  delay_us(500);
  output_float(DQ18S20);
  delay_us(80);
  delay_us(420);
  output_float(DQ18S20);
}

//**************************************************************
void enviaDS18S20(byte dato)
{
 int cnt;
 
 for(cnt=0;cnt<8;++cnt)
 {
  output_low(DQ18S20);
  delay_us(2);
  output_bit(DQ18S20,shift_right(&dato,1,0));
  delay_us(60);
  output_float(DQ18S20);
  delay_us(2);
 }
}

//****************************************************************

byte leeDS18S20()
{
  int cnt,dato;

  for(cnt=0;cnt<8;++cnt)
  {
   output_low(DQ18S20);
   delay_us(2);
   output_float(DQ18S20);
   delay_us(8);
   shift_right(&dato,1,input(DQ18S20));
   delay_us(60);
  }
 return (dato);
}



  
//********************************************************************
float leeTempDS18S20()
{
  
  int8 valor=0;
  
 
  initDS18S20();
  enviaDS18S20(0xCC);  //skip ROM
  enviaDS18S20(0x44);  //prepara conversion
  output_high(DQ18S20);
  while(!leeDS18S20());  //retardo medida
  leeDS18S20();
  initDS18S20();
  enviaDS18S20(0xCC);  //obtiene datos
  enviaDS18S20(0xBE);
  
  valor=leeDS18S20();
  
  return (valor/2);
}  
 
