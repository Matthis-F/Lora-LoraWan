//Librairies recquises :
#include <RHDatagram.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//Définition des pins : 
#define RFM95_CS 8 //Chip select 
#define RFM95_RST 4 //Reset 
#define RFM95_INT 7 //GPIOO/IRQ 
#define RF95_FREQ 868.1 //Notre fréquence

//Définition de l'adresse du node  :
#define ID_NODE_RECEPTEUR 10

//Création du Driver et du Manager 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Driver
RHDatagram manager(rf95, ID_NODE_RECEPTEUR); //Manager

//Setup de l'Arduino : 
void setup() 
{

  pinMode(RFM95_RST, OUTPUT); //Spécification du pin en output
  digitalWrite(RFM95_RST, HIGH); //Paramétrage du voltage

  Serial.begin(9600); //Paramétrage de la liaison série

  delay(100); //Attente

  //Réinitialisation : 
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //Initialisation :
  while (!manager.init()){
    while (1);
  }

  //Paramétrage de la fréquence : 
  if (!rf95.setFrequency(RF95_FREQ)){
    while (1);
  }

  //Paramétrage de la puissance :
  rf95.setTxPower(20,false);
}

//Programme exécuté :
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

  bool res = manager.recvfrom(buf, &len); //Lancement de l'écoute
  Serial.println("Attente de packet ..."); 
  rf95.waitAvailable(); //Bloque l'exécution du code tant que le driver ne reçoit pas de paquets
  //Serial.println(len);
    
  if (res == true){

    Serial.print("Paquet reçu => ");
    Serial.print("Message : ");
    Serial.println((char*)buf);
    
  }
  else {
    Serial.println("Echec de la réception");
  }
   
}

  
  

  
