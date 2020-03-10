//Libraries required : 
#include <RHDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <DHT.h>

//Our RFM95 definition :
#define RFM95_CS 8 //Chip select radio pin
#define RFM95_RST 4 //Reset radio pin
#define RFM95_INT 7 //GPIOO/IRQ radio pin
#define RF95_FREQ 868.1 //European frequency's used
#define DHTPIN 11 // PIN data of DHT11
#define DHTTYPE DHT11 // DHT 11

//Node address definition : 
#define NODE_TERMINAL_ADDRESS 100
#define NODE_RECEPTEUR_ADDRESS 10 

//Fonctions needed : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Driver
RHDatagram manager(rf95, NODE_TERMINAL_ADDRESS); //Manager
DHT dht(DHTPIN, DHTTYPE); //DHT11
 
void setup(){

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
  
  // Connection to the capter
  dht.begin();
}

void loop(){

  delay(5000);
  
  //Temperature in Celsius
  float t = dht.readTemperature();

  //Check if the reading fails, if yes we try again
  if (isnan(t)){
   Serial.println("Failed to read from DHT sensor!");
   return;
  }
  
  //Print the result :
  #ifdef DEBUG
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  #endif
  
  uint8_t data[50];
  char temp[5];
  dtostrf(t,4,1,temp);
  sprintf(data, "Température : %s", temp);
  //Serial.println((char*)data);
    
  Serial.println("Sending to server");
  
    //Sending the message :
    bool res=manager.sendto(data, strlen(data)+1, NODE_RECEPTEUR_ADDRESS);
    
    if (res == true){
      Serial.println("Sending OK");
    }
    else {
      Serial.println("Sending failed ");
    }
  
}
