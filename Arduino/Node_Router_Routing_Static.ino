//Libraries required : 
#include <RHRouter.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//Our RFM95 definition :
#define RFM95_CS 8 //Chip select radio pin
#define RFM95_RST 4 //Reset radio pin
#define RFM95_INT 7 //GPIOO/IRQ radio pin
#define RF95_FREQ 868.1 //European frequency's used

//Node address definition :
//Meaning of the address : number of node + 0(for node without specifity) or 1(for node/server) or 2(for node/hop)
#define CLIENT_ADDRESS 100 
#define ROUTER_ADDRESS 10 
#define GATEWAY_ADDRESS 1 
#define RH_HAVE_SERIAL

//Fonctions needed : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Create an instance of a driver
RHRouter manager_routing(rf95, ROUTER_ADDRESS);

//Arduino's setup : 
void setup() 
{

  pinMode(RFM95_RST, OUTPUT); //Specified pin as an output
  digitalWrite(RFM95_RST, HIGH); //Set the voltage on HIGH

  Serial.begin(9600); //Set the data rate in bits per second for serial data transmission

  delay(100); //Waiting for 100 ms

  //Serial.println("Feather 32u4 Lora Tx Test !");

  //Manual reset : 
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //Initialisation :
  while (!manager_routing.init()){
    while (1);
  }

  //Serial.println("LoRa Radio init OK !");

  //Set our frequency : 
  if (!rf95.setFrequency(RF95_FREQ)){
    while (1);
  }

  //Set the power :
  rf95.setTxPower(20,false);

  //Routing : 
  manager_routing.addRouteTo(CLIENT_ADDRESS, CLIENT_ADDRESS);
  manager_routing.addRouteTo(ROUTER_ADDRESS, ROUTER_ADDRESS);
  manager_routing.addRouteTo(GATEWAY_ADDRESS, GATEWAY_ADDRESS);

}

void loop(){

  //Check the routing table : 
  //manager_routing.printRoutingTable();
  
  //Declaration of variables :
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len=sizeof(buf);
  uint8_t source=manager_routing.headerFrom();
  uint8_t dest=manager_routing.headerTo();
  int8_t rssi=rf95.lastRssi();
  int count = 0;
  
  bool res = manager_routing.recvfromAck(buf, &len, &source, &dest);
  //Serial.println(res);
  
  if (res == 0){
    
    Serial.println("Packet received");
    
  }
  else {
    Serial.println("Reception failed");
  }
  delay(2000);
}

  
  

  
