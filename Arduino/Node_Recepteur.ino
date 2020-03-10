//Libraries required : 
#include <RHDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//Our RFM95 definition :
#define RFM95_CS 8 //Chip select radio pin
#define RFM95_RST 4 //Reset radio pin
#define RFM95_INT 7 //GPIOO/IRQ radio pin
#define RF95_FREQ 868.1 //European frequency's used

//Node address definition :
#define NODE_RECEPTEUR_ADDRESS 10

//Fonctions needed : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Driver
RHDatagram manager(rf95, NODE_RECEPTEUR_ADDRESS); //Manager

//Arduino's setup : 
void setup() 
{

  pinMode(RFM95_RST, OUTPUT); //Specified pin as an output
  digitalWrite(RFM95_RST, HIGH); //Set the voltage on HIGH

  Serial.begin(9600); //Set the data rate in bits per second for serial data transmission

  delay(100); //Waiting for 100 ms

  //Manual reset : 
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //Initialisation :
  while (!manager.init()){
    while (1);
  }

  //Set our frequency : 
  if (!rf95.setFrequency(RF95_FREQ)){
    while (1);
  }

  //Set the power :
  rf95.setTxPower(20,false);
}

void loop(){

  //Serial.println("Ici");
  //Variables :
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len=sizeof(buf);
  
  /*
  uint8_t from=manager.headerFrom();
  uint8_t to=manager.headerTo();
  uint8_t id=manager.headerId();
  uint8_t flags=manager.headerFlags();
  int8_t rssi=rf95.lastRssi();
  */

  bool res = manager.recvfrom(buf, &len);
  Serial.println("Waiting for packets ..."); 
  rf95.waitAvailable();
  //Serial.println(len);
    
  if (res == true){

    Serial.print("Packet received => ");
    Serial.print("Message : ");
    Serial.println((char*)buf);
    
  }
  else {
    Serial.println("Reception failed");
  }
   
}

  
  

  
