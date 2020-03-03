//Libraries required : 
#include <RHReliableDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//Our RFM95 definition :
#define RFM95_CS 8 //Chip select radio pin
#define RFM95_RST 4 //Reset radio pin
#define RFM95_INT 7 //GPIOO/IRQ radio pin
#define RF95_FREQ 868.1 //European frequency's used

//Node address definition :
#define CLIENT1_ADDRESS 100 //The other node
#define CLIENT2_ADDRESS 10 //This node
#define SERVER_ADDRESS 1 //The Gateway 

//Fonctions needed : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Create an instance of a driver
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

//Arduino's setup : 
void setup() 
{

  pinMode(RFM95_RST, OUTPUT); //Specified pin as an output
  digitalWrite(RFM95_RST, HIGH); //Set the voltage on HIGH

  Serial.begin(9600); //Set the data rate in bits per second for serial data transmission

  delay(100); //Waiting for 100 ms

  Serial.println("Feather 32u4 Lora Tx Test !");

  //Manual reset : 
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //Initialisation :
  while (!manager.init()){
    Serial.println("LoRa Radio init failed ... ");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }

  Serial.println("LoRa Radio init OK !");

  //Set our frequency : 
  if (!rf95.setFrequency(RF95_FREQ)){
    Serial.println("Setting Frequency failed ...");
    while (1);
  }

  Serial.print("Set Frequency to : ");
  Serial.print(RF95_FREQ);
  Serial.println(" MHz");

  //Set the power :
  rf95.setTxPower(20,false);
}

void loop(){

  //Declaration of variables :
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len=sizeof(buf);
  uint8_t from=manager.headerFrom();
  uint8_t to=manager.headerTo();
  uint8_t id=manager.headerId();
  uint8_t flags=manager.headerFlags();
  int8_t rssi=rf95.lastRssi();
  int count = 0;
  
  bool res = manager.recvfromAck(buf, &len, &from);
  
  if (res == 0){
    
    Serial.print("Packet received => ");
    Serial.print(rssi);
    Serial.println(" dB");
    Serial.print("From node : ");
    Serial.println(from);
    Serial.print("Message : ");
    Serial.println((char*)buf);
    count = count+1;
    
  }
  else {
    Serial.println("Reception failed");
  }
  Serial.println("Reboot ...");
  delay(1000);
}

  
  

  
