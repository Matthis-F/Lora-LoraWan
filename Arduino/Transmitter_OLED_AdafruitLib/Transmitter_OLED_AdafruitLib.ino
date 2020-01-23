// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// Fréquence à laquelle les données sont envoyées
#define RF95_FREQ 868.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);



//Define for battery
#define VBATPIN A7
void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);

  //Configure Oled Screen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.display();

  //Tant que le moniteur série n'a pas été lancé.
  //while (!Serial) {
    //delay(1);
  //}

  delay(100);

  Serial.println("Feather LoRa TX Test!");
  display.print("Feather LoRa TX Test!");
  display.display();

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    display.print("LoRa radio init failed");
    display.display();
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  display.print("LoRa radio init OK!");
  display.display();

  //Pour que la personne ai le temps de lire ce qu'il y a sur l'écran
  delay(2000);

  //Efface tout le contenu à l'écran et ramène le curseur à son emplacement d'origine
  display.clearDisplay();
  display.setCursor(0,0);

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    display.print("setFrequency failed");
    display.display();
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  display.print("Set Freq to: "); display.println(RF95_FREQ);
  display.display();

  //Pour que la personne ai le temps de lire ce qu'il y a sur l'écran
  delay(2000);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);


   


}



int16_t packetnum = 0;  // packet counter, we increment per xmission



void loop()
{
  //Read battery voltage
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.7;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);
  

  
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
  //Efface tout le contenu à l'écran et ramène le curseur à son emplacement d'origine
  display.clearDisplay();
  display.setCursor(0,0);
  Serial.println("Transmitting..."); // Send a message to rf95_server
  
  char radiopacket[20] = "Hello World";
  Serial.print("Sending "); Serial.println(radiopacket);
  display.print("Send "); display.println(radiopacket);
  display.println(measuredvbat);
  display.display();
  /*radiopacket[19] = 0;
  
  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  Serial.println("Waiting for packet to complete..."); 
  delay(10);*/
  uint8_t data[] = "Hello World";
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC); 

    //Pour l'affichage sur l'écran OLED
      display.print("Got reply: ");
      display.println((char*)buf);
      display.print("RSSI: ");
      display.println(rf95.lastRssi(), DEC); 
    }
    else
    {
      Serial.println("Receive failed");
      display.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
    display.println("No reply, is there a listener around?");
  }

  //Afin d'afficher le texte sur l'écran
  display.display();

}
