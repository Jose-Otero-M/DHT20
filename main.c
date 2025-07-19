#include <16F886.h>
#device ADC=10
#use delay(crystal=4MHz)

#fuses HS,NOWDT,NOPROTECT,NOPUT,NOLVP,NOBROWNOUT,NOMCLR

#use i2c(Master,Fast,sda=PIN_C4,scl=PIN_C3, FORCE_HW, FAST=100000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)
     //baud rate=9800, parity=none, TX=RC6, RX=RC7, data bits=8, ID=PORT1

#include <dht20.c>

#use standard_io(all)

void main(){
   while(dht20_init()); // Wait here until the sensor initializes
   
   float32 hum = 0.0, temp = 0.0;   
   
   while(TRUE){
      delay_ms(5000);    // It is recommended to measure the data every 2 seconds
      
      dht20_get_data(&hum, &temp);
      printf("RH = %0.3F", hum);
      printf("\r\n");
      printf("Temperature %0.3F", temp);
      printf(" *C");
      printf("\r\n");
   }
}
