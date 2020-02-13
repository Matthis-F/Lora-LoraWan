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

RH_RF95 rf95(RFM95_CS, RFM95_INT); //Creating an instance of the RF95 Manager
RHRouter manager_routing(rf95, ROUTER_ADDRESS);//Creating an instance of the
//Routing manager



void setup() 
{

  pinMode(RFM95_RST, OUTPUT); //Specify this pin as an output
  digitalWrite(RFM95_RST, HIGH); //Set the voltage to HIGH

  Serial.begin(9600); //Set the data rate in bits per second for serial 
  //data transmission

  delay(100); //Waiting for 100 ms


  //Manual reset : 
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
  manager_routing.addRouteTo(CLIENT_ADDRESS, CLIENT_ADDRESS);
  manager_routing.addRouteTo(GATEWAY_ADDRESS, GATEWAY_ADDRESS);
//----------------------------------------------------------------------------//

}

//----------------------------------------------------------------------------//
  //Sending or Relaying a Packet
void loop(){

  //Declaration of variables :
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len=sizeof(buf);
  uint8_t source=manager_routing.headerFrom();
  uint8_t dest=manager_routing.headerTo();
  
  //Relaying the message
  bool res = manager_routing.recvfromAck(buf, &len, &source, &dest);
  
  if (res == 0){
    
    Serial.println("Packet received");//If the sendtoWait succeed
    
  }
  else {
    Serial.println("Reception failed");//If the sendtoWait failed 
  }
  //Putting arduino to sleep for 2 sec (2000ms)
  delay(2000);
}

//----------------------------------------------------------------------------//


  
  

  
