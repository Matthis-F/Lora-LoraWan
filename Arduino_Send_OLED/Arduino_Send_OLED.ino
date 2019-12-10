
#include <SX1272.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

//Map buttons to pins (Oled Screen)
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);



void setup() {
  //Configure LORA Module
  sx1272.ON(); //Power on the module
  sx1272._needPABOOST=true; //The module we have need PA_BOOST in order to work correctly
  #define SX1272_debug_mode=2 //Put on Debug mode (1 partial debug) (2 Full debug)
  sx1272.setMode(1);//Lora mode correspond to a predefined combinaison of Spreading Factor and Bandwidth
  sx1272.setPowerDBM((uint8_t)14);//Set MAX Dbm value to 14Dbm
  sx1272.setChannel((uint32_t)CH_10_868);//Using Channel 10 of 868Mhz band for Europe regulation
  sx1272.setNodeAddress(10);//Set node address to 10
  sx1272._enableCarrierSense=true;
 
  //Configure Oled Screen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("<-- Send A");
  display.setCursor(0,20);
  display.println("<-- Send B");
  display.display();
  
  
  Serial.println("Device successfully configured");
  
}
void loop() {
    display.setCursor(0,0);
    sx1272.setPacketType(PKT_TYPE_DATA);//Setting the type of the packet to send here we are sending 
    //DATA so PTK_TYPE_DATE seems appropriate PKT_TYPE8ACK is used for an ACK
    uint8_t r_size;//declare variable r_size 
    int e; //declare variable e
    uint8_t message[100];//declare ariable message
    
    

    if(!digitalRead(BUTTON_A)){
    //reset display
    display.clearDisplay();
    display.setCursor(0,20);
    
    r_size = sprintf((char*)message, "Boutton A");
    Serial.println(e);
    e = sx1272.sendPacketTimeout(2, message, r_size);//Send the Packet
    display.println("Packet Sent !");
    }

    if(!digitalRead(BUTTON_B)){
    //reset display
    display.clearDisplay();
    display.setCursor(0,20);
    
    r_size = sprintf((char*)message, "Boutton B");
    Serial.println(e);
    e = sx1272.sendPacketTimeout(2, message, r_size);//Send the Packet
    display.println("Packet Sent !");
    }
    
    display.display();
    
}
