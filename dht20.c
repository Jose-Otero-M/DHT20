///////////////////////////////////////////////////////////////////////////
////                 DHT20 library for PIC C Compiler                  ////
////                                                                   ////
//// Coded by Otero based on DHT20 Arduino library written by "Rob     ////
//// Tillaart" available on https://github.com/RobTillaart             ////
////                                                                   ////
//// Front view                                                        ////
////                                                                   ////
////            +--------------+                                       ////
////    VDD ----| 1            |                                       ////
////    SDA ----| 2    DHT20   |                                       ////
////    GND ----| 3            |                                       ////
////    SCL ----| 4            |                                       ////
////            +--------------+                                       ////
////                                                                   ////
//// dht20_init()   Must be called before any other function.          ////
////                                                                   ////
////                                                                   ////
////                                                   october, 2022   ////
///////////////////////////////////////////////////////////////////////////


#ifndef uint8_t
   #include <stdint.h> // Used for Standard integer definitions
#endif

int1 dht20_init(void){
   uint8_t request_status_word = 0x71; //(master to slave communication)
   uint8_t status_word;
   delay_ms(100); //After power-on, the sensor needs less than 100ms stabilization time
   i2c_start();
   i2c_write(request_status_word);
   status_word = i2c_read(0);    // It does not send ack
   i2c_stop();
   
   if((status_word | 0x08) == 0) // status word XNOR 0x08
      return 1;                  // Initialize sensor failed
   if(status_word == 255) 
      return 1;                  // Initialize sensor failed
   return 0;
}

void send_trigger_measurement_data(void){
   i2c_start();
   i2c_write(0x70); // I2C addres + write  (slave to master communication)
   //delay_ms(10);    // Is a delay  requerid?
   i2c_write(0xAC); // Trigger measurement 0xAC
   i2c_write(0x33); // Data0
   i2c_write(0x00); // Data1
   i2c_stop();      // End transmission 'P'
}


void dht20_get_data(float32 *humidity, float32 *temperature){

   send_trigger_measurement_data();
   
   static uint8_t data[6]; // Variable is globally active and initialized to 0. Only accessible from this
                           // compilation unit
   uint8_t retries = 3;
   
   while(retries--){
      delay_ms(80);
      i2c_start();
      i2c_write(0x71);       // I2C address + read
      data[0]=i2c_read(1);   // Read byte state and send ack
      if((data[0] >> 7)==0){ // If measurement completed (status bit == 0)
         for(int i = 0; i < 5; i++){
            data[i] = i2c_read(1);  // Read and send ACK
         }
         data[5] = i2c_read(0);    // Read CRC data without send ACK
         i2c_stop();
         break;
      }
      else i2c_stop();
   }
   
   uint32_t raw_data = 0;
   raw_data = data[0];
   raw_data <<= 8;
   raw_data += data[1];
   raw_data <<= 4;
   raw_data += (data[2] >> 4);
   *humidity = raw_data * 9.5367431640625e-5; // ==> raw_data / 1048576.0 * 100%;
   
   raw_data = 0;
   raw_data = (data[2] & 0x0F);
   raw_data <<= 8;
   raw_data += data[3];
   raw_data <<= 8;
   raw_data += data[4];
   *temperature = raw_data * 1.9073486328125e-4 - 50; //  ==> raw_data / 1048576.0 * 200 - 50;
}
