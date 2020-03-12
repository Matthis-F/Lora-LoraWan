//Libraries requises : 
#include <RHRouter.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//Définition des pins :
#define RFM95_CS 8 //Chip select
#define RFM95_RST 4 //Reset
#define RFM95_INT 7 //GPIOO/IRQ

//Déclaration de la fréquence à utiliser :
#define RF95_FREQ 868.1

//Définition de l'adresse des nodes :
#define ID_NODE_TERMINAL 100 
#define ID_NODE_RECEPTEUR 10 
#define ID_GATEWAY 1

//Permet la visualisation de la table de routage :
#define RH_HAVE_SERIAL

//Création du Driver et du Manager : 
RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHRouter manager_routing(rf95, ID_NODE_RECEPTEUR);

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
  while (!manager_routing.init()){
    while (1);
  }

  //Paramétrage de la fréquence : 
  if (!rf95.setFrequency(RF95_FREQ)){
    while (1);
  }

  //Paramétrage de la puissance :
  rf95.setTxPower(20,false);

  //Déclaration de la table de routage : 
  manager_routing.addRouteTo(ID_NODE_TERMINAL, ID_NODE_TERMINAL);
  manager_routing.addRouteTo(ID_NODE_RECEPTEUR, ID_NODE_RECEPTEUR);
  manager_routing.addRouteTo(ID_GATEWAY, ID_GATEWAY);

}

//Programme exécuté :
void loop(){

  //Affiche la table de routage : 
  //manager_routing.printRoutingTable();
  
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
  
  bool res = manager_routing.recvfromAck(buf, &len); //Lancement de l'écoute des paquets
  Serial.println("Attente de packet ..."); 
  rf95.waitAvailable(); //Bloque l'exécution du code tant que le driver ne reçoit pas de paquets
  
  if (res == true){

    Serial.print("Paquet reçu => ");
    Serial.print("Message : ");
    Serial.println((char*)buf);
    
  }
  else {
    Serial.println("Echec de la réception");
  }
  
}

  
  

  
