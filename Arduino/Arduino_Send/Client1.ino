//This code is only to send a message to a NODE with and ACK
//We will call this node CLIENT1
//The destination is either CLIENT2 or GATEWAY
//But here, the destination is CLIENT2
//In fact, it's in order to understand how work the exchange of packets between nodes with LORA
//Here, CLIENT1 is sending packets
//So we will see how to code the ending
//This file is also the base file for the routing

//----------------------------------------------------------------------------//

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
#define CLIENT1_ADDRESS 100 //This node 
#define CLIENT2_ADDRESS 10 //The other node
#define GATEWAY_ADDRESS 1 //The gateway

//----------------------------------------------------------------------------//

//Instances needed : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Create an instance of a RH_RF95
RHReliableDatagram manager(rf95, CLIENT1_ADDRESS); //Create an instance of a RHReliableDatagram

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
  rf95.setTxPower(20,false); //Our module is using PA_BOOST so we can go up to 20dBm
  //Unless your radio module can take advantage of PA_BOOST set this value to 13dBm and set "true"
  
  //----------------------------------------------------------------------------//
  
}

//----------------------------------------------------------------------------//

//Code for transmission with ACK : 

void loop(){
  
  uint8_t data[] = "Hello World !"; //Message to send
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //Buffer
  uint8_t len = sizeof(buf); //Size of the buffer
  
  manager.setTimeout(200); //After each packet or retry the program waits
  //Between 200ms and 400ms (used to avoid colision
  //setTimeout(200) is the default value
  //That means : if you forgot to define it explicitly, the timeout is 200ms 
  
  manager.setRetries(3); //If a messages is not acquired by the recipient
  //The manager while try 3 times before giving up the packet
  //setRetries(3) is a default value
  //That means : if you forgot to define it explicitly, the number of retries is 3  
  
  //Sending the message :
  bool res=manager.sendtoWait(data, sizeof(data), CLIENT2_ADDRESS);
  //The funtion "sendtoWait" will send the data to the DESTINATION ADDRESS
  //Here it's the second node but, CLIENT2_ADDRESS, it could be the GATEWAY_ADDRESS
  //In parameters, we have to put our message, the size ot the message, and the destination
  //Once executed, the function will return a boolean, 0 or 1
  //These values will help us to see if the packets are correctly received

  //There is an ACK for each sent message thanks to the variable "res"
  //The value of "res" will help us to verify if the message was transmitted or not
  if (res == 1){
    
    Serial.println("Sending OK"); //If the sendtoWait worked 
            
  }
  else {
    
    Serial.println("Sending failed "); //If the sendtoWait failed
    
  }
  //Putting Arduino to sleep for 2 sec (2000ms)
  delay(2000);

}

//----------------------------------------------------------------------------//
