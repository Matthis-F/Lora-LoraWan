
#include <SX1272.h>
#include <SPI.h>
#include <Wire.h>




void setup() {
  //Configure LORA Module
  sx1272.ON(); //Power on the module
  sx1272._needPABOOST = true; //The module we have need PA_BOOST in order to work correctly
#define SX1272_debug_mode=2 //Put on Debug mode (1 partial debug) (2 Full debug)
  sx1272.setMode(1);//Lora mode correspond to a predefined combinaison of Spreading Factor and Bandwidth
  sx1272.setPowerDBM((uint8_t)14);//Set MAX Dbm value to 14Dbm
  sx1272.setChannel((uint32_t)CH_10_868);//Using Channel 10 of 868Mhz band for Europe regulation
  sx1272.setNodeAddress(2);//Set node address to 10
  sx1272._enableCarrierSense = true; //enable carrier sense in case of debugging needed


  Serial.println("Device successfully configured");

}
void loop() {

  int e;
  String data;
  
  e = sx1272.receivePacketTimeout(10000);//Open a 10s time window
  

  switch (e) {

    case 0:

      int packet_length = sx1272.packet_received.length;//return legth of received packet
      Serial.println("Incoming Packet");

      for (unsigned int i = 0; i < (packet_length)-1 ; i++)//Used to read payload data (returned as char array) and put it into a String
      {
        data = data + ((char)sx1272.packet_received.data[i]);
      }
      Serial.print("Payload of packet :");
      Serial.println(data);//prints payload
      
      break;

 

    case 3:
      Serial.println("No packet received during time window");
      break;
    default:
      char msg_error;
      sprintf(msg_error,"An error occured, error code:%d",e);
      Serial.println(msg_error);
      

  }  
}
