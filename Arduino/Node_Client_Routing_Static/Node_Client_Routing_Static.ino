//----------------------------------------------------------------------------//
//Libraries required :
#include <RHRouter.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <LoraWanPkt.h>
#include <Base64.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
//----------------------------------------------------------------------------//

uint8_t data_in[50] = {0};
unsigned char data_out[100];
uint8_t dir = 0;

char humi[5];
char temp[5];

#define DHTPIN 11
#define DHTTYPE DHT11   // DHT 11 

//#define DEBUG //used for debugging only

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

DHT dht = DHT(DHTPIN, DHTTYPE);


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
  while (!manager_routing.init()) {
    while (1);
  }

  //Set our frequency :
  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }

  //Set the power :
  rf95.setTxPower(20, false); //Our module is using PA_BOOST so we can go up to 20dBm
  //Unless your radio module can take advantage of PA_BOOST set this value to 13dBm

  //----------------------------------------------------------------------------//
  //Static Routing table :
  //Routing schema:
  // CLIENT_ADDRESS <-> ROUTER_ADDRESS <-> GATEWAY_ADDRESS
  //To join the DESTINATION, we have to pass through the NEXT HOP
  manager_routing.addRouteTo(ROUTER_ADDRESS, ROUTER_ADDRESS); //To join ROUTER_ADDRESS -> next hop : ROUTER_ADDRESS
  manager_routing.addRouteTo(GATEWAY_ADDRESS, ROUTER_ADDRESS); //To join GATEWAY_ADDRESS -> next hop : ROUTER_ADDRESS
  //----------------------------------------------------------------------------//
  dht.begin();

}

//----------------------------------------------------------------------------//

//Sending the packet

void loop() {
  uint16_t FrameCount = 0x0000;
  float t = dht.readTemperature();//reading the temp from the sensor
  dtostrf(t,5,2,temp);//Float to string
  sprintf(data_in,"{msg:{Node:%d,Temperature:%s}}",CLIENT_ADDRESS,temp);
  uint8_t data_len = strlen((char*)data_in);//putting string into data buffer

#ifdef DEBUG
  Serial.print("data len :");
  Serial.print(data_len);
  Serial.println();
#endif


  uint8_t packet_len = buildLoraWanPkt(data_in, data_len, dir, FrameCount);

#ifdef DEBUG
  Serial.print("packet len :");
  Serial.print(packet_len);
  Serial.println();
#endif

  uint8_t base64_len = base64_encode(data_out, data_in, packet_len);

  manager_routing.setTimeout(200); //After each packet or retry the program waits
  //Between 200ms and 2 times this value (400ms) this behaviour is used to avoid
  //colisions. (e.g when 2 Node sends a packet at the same time)
  //Note that the default value is 200ms

  manager_routing.setRetries(3); //If a messages is not acquired by the recipient
  //The manager while try 3 times before giving up sending the packet
  //3 is the default value.

  //Sending the message :
  bool res = (manager_routing.sendtoWait(data_out, base64_len + 1, GATEWAY_ADDRESS) == RH_ROUTER_ERROR_NONE);
  //We use the same function "sendtoWait" as declared in the RHDatagram ...
  //But our destination address is "GATEWAY_ADDRESS" and we add "==RH_ROUTER_ERROR_NONE"
  //"==RH_ROUTER_ERROR_NONE" : message was routed and delivered to the next-hop
  //Using the routing table declared above, the packet will be forwarded following the routing table

  //Check the result of the "res"
  if (res == 0) {

    Serial.println("Sending Failed "); //If the sendtoWait succeed
    //For more informations about who received the packet
    //Use the same code that was written in

  }
  else {

    Serial.println("Sending OK"); //If the sendtoWait failed

  }


  //----------------------------------------------------------------------------//
  //only used for debugging through Serial
#ifdef DEBUG
  for (uint8_t i = 0; i < packet_len; i++) {
    Serial.print(data_in[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif
  //----------------------------------------------------------------------------//


  //Putting arduino to sleep for 2 sec (2000ms)
  delay(2000);

}

//----------------------------------------------------------------------------//
