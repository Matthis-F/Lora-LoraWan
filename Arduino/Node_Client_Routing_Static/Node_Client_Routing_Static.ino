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

#define RH_HAVE_SERIAL


RH_RF95 rf95(RFM95_CS, RFM95_INT);//Creating an instance of the RF95 Manager
RHRouter manager_routing(rf95, CLIENT_ADDRESS); //Creating an instance of the
//Routing manager


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



//----------------------------------------------------------------------------//
 //Static Routing table :
  manager_routing.addRouteTo(ROUTER_ADDRESS, ROUTER_ADDRESS);
  manager_routing.addRouteTo(GATEWAY_ADDRESS, ROUTER_ADDRESS);
//----------------------------------------------------------------------------//

}


//----------------------------------------------------------------------------//
  //Sending the packet
void loop(){

  uint8_t data[] = "Hello World!"; //Message to send
  
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; //Buffer
  uint8_t len = sizeof(buf); //Size of the buffer
  
  manager_routing.setTimeout(200);//After each packet or retry the program waits
  //between 200ms and 400ms (used to avoid colision)
 
  manager_routing.setRetries(3);//If a messages is not acquired by the recipient
  //the manager while try 3 times before giving up the packet
  
  //Sending the message :
  bool res=(manager_routing.sendtoWait(data, sizeof(data), GATEWAY_ADDRESS)==RH_ROUTER_ERROR_NONE);
  
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
