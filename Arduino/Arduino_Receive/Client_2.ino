//Libraries required : 
#include <RHReliableDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//----------------------------------------------------------------------------//

//Our RFM95 definition :
#define RFM95_CS 8 //Chip select radio pin
#define RFM95_RST 4 //Reset radio pin
#define RFM95_INT 7 //GPIOO/IRQ radio pin
#define RF95_FREQ 868.1 //European frequency's used

//----------------------------------------------------------------------------//

//Node address definition :
#define CLIENT1_ADDRESS 100 //The other node
#define CLIENT2_ADDRESS 10 //This node
#define SERVER_ADDRESS 1 //The Gateway 

//----------------------------------------------------------------------------//

//Fonctions needed : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Create an instance of a driver
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

//----------------------------------------------------------------------------//

//Arduino's setup : 
void setup() 
{

  pinMode(RFM95_RST, OUTPUT); //Specified pin as an output
  digitalWrite(RFM95_RST, HIGH); //Set the voltage on HIGH

  Serial.begin(9600); //Set the data rate in bits per second for serial data transmission

  delay(100); //Waiting for 100 ms

//----------------------------------------------------------------------------//
  
  //Manual reset : 
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

//----------------------------------------------------------------------------//  
  
  //Initialisation :
  while (!manager.init()){
    while (1);
  }

//----------------------------------------------------------------------------//  
  
  //Set our frequency : 
  if (!rf95.setFrequency(RF95_FREQ)){
    while (1);
  }

//----------------------------------------------------------------------------//
  
  //Set the power :
  rf95.setTxPower(20,false);//Our module is using PA_BOOST so we can go up to 20dBm
  //Unless your radio module can take advantage of PA_BOOST set this value to 13dBm and set "true"
}

//----------------------------------------------------------------------------//

//Code for reception of packets :
void loop(){

  //Declaration of variables :
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //Buffer
  uint8_t len=sizeof(buf); //Size of buffer
  uint8_t from=manager.headerFrom(); //Returns the FROM header of the last received message
  uint8_t to=manager.headerTo(); //Returns the TO header of the last received message
  uint8_t id=manager.headerId(); //Returns the ID of the last received message
  uint8_t flags=manager.headerFlags(); //Returns the FLAGS of the last received message
  int8_t rssi=rf95.lastRssi(); //Returns the POWER of the last received message
  int count = 0; //Counts the number of packets received
  
  //Receiving the message :
  bool res = manager.recvfromAck(buf, &len, &from);
  //The function "recvfromAck" will listen for packets which are for it
  //Here, it will listen for packets from CLIENT_1
  //The fonction takes, in parameters, the buffer, the lenght of buffer, and the source
  //Once executed, the function will return a boolean, 0 or 1
  //These values will help us to see if the the packets are correctly received
  
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
  //Putting Arduino to sleep for 1 sec (1000ms)
  delay(1000);
}

//----------------------------------------------------------------------------//
