
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include "Chaplex.h"
const int LED_1 = 13;     //LED row 1
const int LED_2 = 12;     //LED row 2
const int LED_3 = 10;     //LED row 3

//chaplex stuff
byte controlPins[] = {13, 12, 10};
#define numControlPins sizeof(controlPins) / sizeof(*controlPins)
Chaplex myCharlie(controlPins, numControlPins); //control instance
charlieLed myLeds[6] = {
  { 2 , 1 },    // 1 controlled leds in sorted order (when looking at front, from left -> to -> right)
  { 1 , 0 },    // 2 every element is one led with
  { 1 , 2 },    // 3 {anode-pin,cathode-pin}
  { 0 , 1 },    // 4 "pin" means here - index in controlPins
  { 2 , 0 },    // 5 array defined above
  { 0 , 2 }     // 6
};

const char *ssid = "BadgePiratesAP_01"; // The name of the Wi-Fi network that will be created
const char *password = "thereisnospoon";   // The password required to connect to it, leave blank for an open network
const int dTime = 50;
const int WifiFlag = 0;
int gameEnabled = 0;
int WifiFlags[] = {0,0,0,0,0,0};
int completedArray[] = {1,1,1,1,1,1};

bool gameCompleted = false;

long goneTime;
#define NEWPATTERN 100        //100 ms for new LED pattern
os_timer_t myTimer;
bool tickOccured;
bool reverseAnimation = false;

void timerCallback(void *pArg) {
  myCharlie.outRow();
} // End of timerCallback

void setup() {
  Serial.begin(74880);
  delay(10);
  Serial.println();
  os_timer_setfn(&myTimer, timerCallback, NULL);
  os_timer_arm(&myTimer, 5, true);
  randomSeed(analogRead(0));
  goneTime = millis();

  if (WifiFlag == 1) {
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    Serial.print("Wifi Disabled \"");
  }

  if (WifiFlag ==0){
    //WiFi.softAP(ssid, password);             // Start the access point
    Serial.print("Access Point \"");
    Serial.print(ssid);
    Serial.println("\" setup but not started");

    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the Monitor

    Serial.print("MAC:\t ");
    Serial.println(WiFi.macAddress());  // Send the MAC address of the ESP8266 to the Monitor
    Serial.println('\n');

    listNetworks();
  }
}


void loop()
{
  // const unsigned long fiveMinutes = 5 * 60 * 1000UL;
  const unsigned long fiveMinutes = 15 * 1000UL;  // 15 seconds to test, uncomment above code for 5 minutes
  static unsigned long lastSampleTime = 0 - fiveMinutes;  // initialize such that a reading is due the first time through loop()
  unsigned long now = millis();
  if (now - lastSampleTime >= fiveMinutes)
  {
     Serial.print("Scanning:\t ");
     lastSampleTime += fiveMinutes;
     if(!gameCompleted){
       listNetworks();
     }
  }

  // add code to do other stuff here
  //single led
  // myCharlie.ledWrite(lights[0], ON);
  // myCharlie.outRow();

  //flicker pattern
  // if (millis()-goneTime >= NEWPATTERN) {
  //   for (byte i=0; i<10; i++)
  //     myCharlie.ledWrite(lights[i], (byte)random(0,2));
  //   goneTime = millis();
  // }
  // myCharlie.outRow();

  if(gameEnabled == 1) {

    if(gameCompleted){
      //flicker pattern
      if (millis()-goneTime >= NEWPATTERN) {
        for (byte i=0; i<10; i++)
          myCharlie.ledWrite(myLeds[i], (byte)random(0,2));
        goneTime = millis();
      }
    }
    else{
      if(WifiFlags[0] == 1){
        myCharlie.ledWrite(myLeds[0], ON);
      } else {
        myCharlie.ledWrite(myLeds[0], OFF);
      }

      if(WifiFlags[1] == 1){
        myCharlie.ledWrite(myLeds[1], ON);
      } else {
        myCharlie.ledWrite(myLeds[1], OFF);
      }

      if(WifiFlags[2] == 1){
        myCharlie.ledWrite(myLeds[2], ON);
      } else {
        myCharlie.ledWrite(myLeds[2], OFF);
      }

      if(WifiFlags[3] == 1){
        myCharlie.ledWrite(myLeds[3], ON);
      } else {
        myCharlie.ledWrite(myLeds[3], OFF);
      }

      if(WifiFlags[4] == 1){
        myCharlie.ledWrite(myLeds[4], ON);
      } else {
        myCharlie.ledWrite(myLeds[4], OFF);
      }

      if(WifiFlags[5] == 1){
        myCharlie.ledWrite(myLeds[5], ON);
      } else {
        myCharlie.ledWrite(myLeds[5], OFF);
      }
    }
  }
  else {
    if(!reverseAnimation) {
      for (int i=0; i < 6; i++) {
        for(int x=0; x < 6; x++){
          if(x == i) { myCharlie.ledWrite(myLeds[x], ON); }
          else {myCharlie.ledWrite(myLeds[x], OFF);}
        }
        delay(dTime);
      }
      reverseAnimation = true;
    }
    else {
      for (int i=5; i > 0; i--) {
        for(int x=5; x > 0; x--){
          if(x == i) { myCharlie.ledWrite(myLeds[x], ON); }
          else {myCharlie.ledWrite(myLeds[x], OFF);}
        }
        delay(dTime);
      }
      reverseAnimation = false;
    }
  }
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  String badgePirateSSID = "badgepirates";
  // iterate over each ssid, then iterate over each led to check if we should
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    String thisSSID = WiFi.SSID(thisNet);
    for(int ssidIndex = 0; ssidIndex <= 5; ssidIndex++){
      if( thisSSID == badgePirateSSID + ssidIndex){
        gameEnabled = 1;
        WifiFlags[ssidIndex] = 1;
        Serial.print("found batman and robin" + ssidIndex);
      }
    }
  }

  gameCompleted = checkArrays(WifiFlags, completedArray, 6);
}

boolean checkArrays(int arrayA[],int arrayB[], long numItems) {
  boolean same = true;
  long i = 0;
  while(i<numItems && same) {
    same = arrayA[i] == arrayB[i];
    i++;
  }
  return same;
}
