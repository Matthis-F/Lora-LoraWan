//Libraries requises :
#include <RHRouter.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

//Définition des pins :
#define RFM95_CS 8 //Chip select
#define RFM95_RST 4 //Reset
#define RFM95_INT 7 //GPIOO/IRQ
#define RF95_FREQ 868.1 //Notre fréquence
#define DHTPIN 11 //PIN data
#define DHTTYPE DHT11 //Modèle du notre capteur : DHT 11

//Déclaration de la fréquence à utiliser :
#define RF95_FREQ 868.1

//Définition de l'adresse des nodes :
#define ID_NODE_TERMINAL 100
#define ID_NODE_RECEPTEUR 10
#define ID_GATEWAY_ADDRESS 1

//Permet la visualisation de la table de routage :
#define RH_HAVE_SERIAL

//Création du Driver, du Manager et du capteur : 
RH_RF95 rf95(RFM95_CS, RFM95_INT); //Driver
RHReliableDatagram manager(rf95, NODE_TERMINAL_ADDRESS); //Manager
DHT dht(DHTPIN, DHTTYPE); //Capteur

//Setup de l'Arduino :
void setup(){

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
  
  //Connexion au capteur :
  dht.begin();
}

//Programme exécuté :
void loop(){

  //Récupération de la température en Celsius :
  float t = dht.readTemperature();

  //Si la récupération échoue, on recommence :
  if (isnan(t)){
   Serial.println("Récupération de la température échouée!");
   return;
  }
  
  //Affiche la température récupérée pour le debug :
  #ifdef DEBUG
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  #endif
  
  uint8_t data[50]; //Taille du tableau 
  char temp[5]; //Taille du tableau
  dtostrf(t,4,1,temp); //Convertie la varibale t en chaine de caractère
  sprintf(data, "Température : %s", temp); //Forme une chaine de caractère
  //Serial.println((char*)data); //Affiche la chaine de caractère
  
  //manager.setRetries(2); //Modification du nombre d'essai
  //manager.setTimeout(500); //Modification du temps d'attente entre deux essais
    
  Serial.println("Envoie au serveur");
  
    //Transmission du message :
    bool res=manager.sendtoWait((data, strlen(data)+1, ID_GATEWAY) == RH_ROUTER_ERROR_NONE);
    
    if (res == true){
      Serial.println("Transmission OK");
    }
    else {
      Serial.println("Transmission échouée");
    }
  
  delay(5000); //Temps d'attente avant ré-émission
  
}
