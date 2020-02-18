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
#define CLIENT1_ADDRESS 100 //This node 
#define CLIENT2_ADDRESS 10 //The other node
#define GATEWAY_ADDRESS 1 //The gateway

//Fonctions needed : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Create an instance of a driver
RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

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

//Code for transmission with ACK : 
void loop(){
  
  Serial.println("Sending to server");
  uint8_t data[] = "Hello World node 22!"; //Message to send
  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //Buffer
  uint8_t len = sizeof(buf); //Size of the buffer
  //manager.setTimeout(1000); //Timeout after each retry but timeout = [timeout;timeout*2]
  //manager.setRetries(0);
  //Sending the message :
  bool res=manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS);
  Serial.println(res);

  if (res == 0){
    Serial.println("Sending failed"); //If the sendtoWait failed 
  }
  else {
    Serial.println("Sending OK "); //If the sendtoWait succeed
  }
  //Delay before the next round :
  Serial.println("Reboot ..."); 
  delay(2000); //2 sec

}
