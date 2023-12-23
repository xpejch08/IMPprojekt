#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RDA5807.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//oled ports
#define OLED_MOSI   23
#define OLED_CLK   18
#define OLED_DC    27
#define OLED_CS    5
#define OLED_RESET 17


RDA5807 radio;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


const int CLK_PIN = 14;  // CLK pin of KY-040 connected to GPIO 14
const int DT_PIN = 13;   // DT pin of KY-040 connected to GPIO 13
const int SW_PIN = 25; // SW pin of KY-040 connected to GPIO 25


void setup() {
  Wire.begin();
  Serial.begin(115200);
  // Set pins as inputs and turn on pullup rezistor
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT_PULLUP);
  pinMode(SW_PIN, INPUT_PULLUP);

  //DISPLAY INICIALIZATION
  Serial.println("Initializing OLED display...");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Serial.println("Clearing display...");
  display.clearDisplay();
  
  Serial.println("Setting up text properties...");
  display.drawPixel(10, 10, SSD1306_WHITE);

  
  display.display();
  delay(2000);


  //FM MODULE INICIALIZATION
  radio.setup(); 
  radio.setRDS(true);
  radio.setAFC(true);
  radio.setRdsFifo(true);
  radio.setVolume(6);
  radio.setFrequency(9930); // Tune evropa 2 as default

  Serial.print("Current frequency: ");
  Serial.println(99.30, 1);
}

void showFrequency() {
   uint16_t freq = radio.getFrequency();
   Serial.print(freq);
   Serial.println("MHz ");
}


void displayInfo(char* stationName, char* stationInfo, char* programInfo, char* rdsTime){
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(stationName);
  display.println(stationInfo);
  display.println(programInfo);
  display.println(rdsTime);
  display.display();
}

void loop(){

    char *stationName, *stationInfo, *programInfo, *rdsTime;
    static int lastStateCLK = digitalRead(CLK_PIN);
    int currentStateCLK = digitalRead(CLK_PIN);
    static int lastStateDT = lastStateCLK; 
    int currentStateDT = digitalRead(DT_PIN);


    //seeking condition
    if (digitalRead(SW_PIN) == LOW){
          display.clearDisplay(); 
          display.setTextSize(1);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(0,0);
          display.println("Seeking!");
          display.display();
          radio.seek(RDA_SEEK_WRAP, RDA_SEEK_UP, showFrequency);
          display.clearDisplay(); 
          display.setTextSize(1);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(0,0);
          display.println("Found!");
          display.display();
    }

    
        display.clearDisplay(); 
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println("Frequency:");
        display.println(radio.getFrequency()); // Print the new frequency
        if(radio.getRdsAllData(&stationName, &stationInfo , &programInfo, &rdsTime)){
          if(stationName != NULL){
            display.println(stationName);
          }
          if(stationInfo != NULL){
            display.println(stationInfo);
          }
          if(programInfo != NULL){
            display.println(programInfo);
          }
          if(rdsTime != NULL){
            display.println(rdsTime);
          }
        }
        display.display();

    if (digitalRead(CLK_PIN) == HIGH && digitalRead(DT_PIN) == LOW) {
            // Clockwise rotation
            radio.setFrequency(radio.getFrequency() + 10);
            Serial.print("Right");
          if (radio.getRdsAllData(&stationName, &stationInfo , &programInfo, &rdsTime) ) {
            Serial.print("Station name: ");
            Serial.println(stationName);
            Serial.println(stationInfo);
            Serial.println(programInfo);
            Serial.println(rdsTime);
          }
        } else if(digitalRead(CLK_PIN) == LOW && digitalRead(DT_PIN) == HIGH) {
            // Counter-clockwise rotation
            radio.setFrequency(radio.getFrequency() - 10);
            Serial.print("LEFT");
            if (radio.getRdsAllData(&stationName, &stationInfo , &programInfo, &rdsTime) ) {
            Serial.print("Station name: ");
            Serial.println(stationName);
            Serial.println(stationInfo);
            Serial.println(programInfo);
            Serial.println(rdsTime);
            }
        }
        Serial.println(radio.getFrequency());


    delay(10);



}