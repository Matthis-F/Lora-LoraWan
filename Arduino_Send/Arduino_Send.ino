#include <SX1272.h>

void setup() {
  // put your setup code here, to run once:
  sx1272.ON(); //Power on the module
  sx1272._needPABOOST=true; //The module we have need PA_BOOST in order to work correctly
  #define SX1272_debug_mode=2 //Put on Debug mode (1 partial debug) (2 Full debug)
  sx1272.setMode(1);//Lora mode correspond to a predefined combinaison of Spreading Factor and Bandwidth
  sx1272.setPowerDBM((uint8_t)14);//Set MAX Dbm value to 14Dbm
  sx1272.setChannel((uint32_t)CH_10_868);//Using Channel 10 of 868Mhz band for Europe regulation
  sx1272.setNodeAddress(10);//Set node address to 10
  sx1272._enableCarrierSense=true;
  Serial.println("Device successfully configured");
  
}
void loop() {
  sx1272.CarrierSense();//need to be enabled for listen before talk feature
  sx1272.setPacketType(PKT_TYPE_DATA);//Setting the type of the packet to send here we are sending 
  //DATA so PTK_TYPE_DATE seems appropriate PKT_TYPE8ACK is used for an ACK
  uint8_t r_size;//declare variable r_size 
  int e; //declare variable e
  uint8_t message[100];//declare ariable message
  while (1){

    r_size = sprintf((char*)message, "Ping");
    e = sx1272.sendPacketTimeout(2, message, r_size);//Sendinf the Packet
    Serial.println(e);
    delay(1000);//set a 1sec delay between each packet
  
  }
  
}
