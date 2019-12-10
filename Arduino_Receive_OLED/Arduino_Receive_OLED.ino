
#include <SX1272.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

//Map buttons to pins (Oled Screen)
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5

int e;

void setup() {
  //Configure Oled Screen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  //Clear display buffer
  display.clearDisplay();
  display.display();
  
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Ready");
  display.display(); //Display all of the above
  
  //Configure LORA Module
  sx1272.ON(); //Power on the module
  sx1272._needPABOOST = true; //The module we have need PA_BOOST in order to work correctly
  sx1272.setMode(1);//Lora mode correspond to a predefined combinaison of Spreading Factor and Bandwidth
  sx1272.setPowerDBM((uint8_t)14);//Set MAX Dbm value to 14Dbm
  sx1272.setChannel((uint32_t)CH_10_868);//Using Channel 10 of 868Mhz band for Europe regulation
  sx1272.setNodeAddress(2);//Set node address to 10
  sx1272._enableCarrierSense = true; //enable carrier sense in case of debugging needed

  
  Serial.println("Device successfully configured");
  display.println("Conf: OK");
  display.display();

}
void loop() {
  
  e = sx1272.receivePacketTimeout(10000);//Open a 10s time window
  Serial.println(e);

  if( e == 0 ){
      //Clear Display
      display.clearDisplay();
      display.setCursor(0,0);
      
      String data;
      int packet_nb = sx1272.packet_received.packnum;
      int packet_length = sx1272.packet_received.length;//return legth of received packet
      Serial.println("Incoming Packet");
  
      for (unsigned int i = 0; i < (packet_length)-1 ; i++)//Used to read payload data (returned as char array) and put it into a String
      {
        data = data + ((char)sx1272.packet_received.data[i]);
      }
      Serial.print("Payload of packet :");
      Serial.println(data);//prints payload
      Serial.println(packet_nb);
      display.println("Packet Received");
      display.print("Payload: ");
      display.println(data);
      display.print("Packet nb: ");
      display.println(packet_nb);
  }
  else if( e == 3 ){
      Serial.println("No packet received during time window");
      display.clearDisplay();
      display.setCursor(0,0);     
      display.println("No Packet received");
  }
 
   else{
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("An error Occured");
      char msg_error;
      sprintf(msg_error,"An error occured, error code:%d",e);
      Serial.println(msg_error);
   }
      
  display.display();
}
