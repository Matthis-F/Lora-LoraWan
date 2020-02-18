//Libraries required : 
#include <RHRouter.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//----------------------------------------------------------------------------//

//Our RFM95 definition :
#define RFM95_CS 8 //Chip select radio pin
#define RFM95_RST 4 //Reset radio pin
#define RFM95_INT 7 //GPIO/IRQ radio pin

//----------------------------------------------------------------------------//

#define RF95_FREQ 868.1 //Match EU regulations

//----------------------------------------------------------------------------//

//Node address definition :
#define CLIENT_ADDRESS 100
#define ROUTER_ADDRESS 10
#define GATEWAY_ADDRESS 1

//----------------------------------------------------------------------------//

RH_RF95 rf95(RFM95_CS, RFM95_INT); //Creating an instance of the RF95 Manager
RHRouter manager_routing(rf95, CLIENT_ADDRESS); //Creating an instance of the routing manager

void setup() 
{

  pinMode(RFM95_RST, OUTPUT); //Specify this pin as an output
  digitalWrite(RFM95_RST, HIGH); //Set the voltage to HIGH

  Serial.begin(9600); //Set the data rate in bits per second for serial data transmission

  delay(100); //Waiting for 100 ms

  //Manual reset : s
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);


//----------------------------------------------------------------------------//
  
  //Initialisation :
  while (!manager_routing.init()){
    while (1);
  }

  //Set our frequency : 
  if (!rf95.setFrequency(RF95_FREQ)){
    while (1);
  }

  //Set the power :
  rf95.setTxPower(20,false);//Our module is using PA_BOOST so we can go up to 20dBm
  //Unless your radio module can take advantage of PA_BOOST set this value to 13dBm

//----------------------------------------------------------------------------//
  
  //Static Routing table :
  //Routing schema:
  // CLIENT_ADDRESS <-> ROUTER_ADDRESS <-> GATEWAY_ADDRESS
  //To join the DESTINATION, we have to pass through the NEXT HOP
  manager_routing.addRouteTo(ROUTER_ADDRESS, ROUTER_ADDRESS); //To join ROUTER_ADDRESS -> next hop : ROUTER_ADDRESS
  manager_routing.addRouteTo(GATEWAY_ADDRESS, ROUTER_ADDRESS); //To join GATEWAY_ADDRESS -> next hop : ROUTER_ADDRESS
  //Here to join the GATEWAY_ADDRESS, the packets have to go to the ROUTER_ADDRESS that's know how to join the GATEWAY_ADDRESS
  //This is the routing : to join the destination, we have to "send" the packet to the next-hop that's could know the route
  
//----------------------------------------------------------------------------//

}

//----------------------------------------------------------------------------//

//Sending the packet

void loop(){

  uint8_t data[] = "Hello World!"; //Message to send
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //Buffer
  uint8_t len = sizeof(buf); //Size of the buffer
  
  manager_routing.setTimeout(200); //After each packet or retry the program waits
  //Between 200ms and 400ms (used to avoid colision
  //setTimeout(200) is the default value
  //That means : if you forgot to define it explicitly, the timeout is 200ms 
 
  manager_routing.setRetries(3); //If a messages is not acquired by the recipient
  //The manager while try 3 times before giving up the packet
  //setRetries(3) is a default value
  //That means : if you forgot to define it explicitly, the number of retries is 3  
  
  //Sending the message :
  bool res=(manager_routing.sendtoWait(data, sizeof(data), GATEWAY_ADDRESS)==RH_ROUTER_ERROR_NONE);
  //We use the same function "sendtoWait" as declared in the RHDatagram ...
  //But our destination address is "GATEWAY_ADDRESS" and we add "==RH_ROUTER_ERROR_NONE"
  //"==RH_ROUTER_ERROR_NONE" : message was routed and delivered to the next-hop
  //Using the routing table declared above, the packet will be forwarded following the routing table
  //The packets know where to go to join the destination, here GATEWAY_ADDRESS
  
  //Check the result of the "res"
  if (res == 0){
    
    Serial.println("Sending OK "); //If the sendtoWait succeed
    
  }
  else {
    
    Serial.println("Sending failed"); //If the sendtoWait failed 
    
  }
  //Putting arduino to sleep for 2 sec (2000ms)
  delay(2000);
  
}

//----------------------------------------------------------------------------//
