
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Chaplex.h"

//THE PARTICIPANT BADGES HAVE DIFFERENT MODEL ESP8266. PLEASE READ THIS SECTION CAREFULLY IF YOU ARE USING
//A NON-PARTICIPANT BADGE

//The leds use a method called 'Charlieplexing' for control
//this allows us to control 6 leds with only 3 pin outs
//we use a library called 'Chaplex' to help with some of the complexity
//for more information on the fascinating world of charlieplexing,
//checkout the wikipedia: https://en.wikipedia.org/wiki/Charlieplexing

//setup chaplex
byte controlPins[] = {13, 10, 12}; //{13, 12, 10} <--- use this if you do not have a participant badge
#define numControlPins sizeof(controlPins) / sizeof(*controlPins)
Chaplex myCharlie(controlPins, numControlPins); //control instance
charlieLed scoreLeds[6] = {
  { 2 , 1 },    // 1 controlled leds in sorted order (when looking at front, from left -> to -> right)
  { 0 , 2 },    // 2 every element is one led with
  { 1 , 2 },    // 3 {anode-pin,cathode-pin}
  { 2 , 0 },    // 4 "pin" means here - index in controlPins
  { 0 , 1 },    // 5 array defined above
  { 1 , 0 }     // 6
};

//IF YOU DO NOT HAVE A WHITE PARTICIPANT BADGE, UN-COMMENT THE scoreLeds DECLARATION BELOW AND COMMENT OUT THE ONE ABOVE

// charlieLed scoreLeds[6] = {
//   { 2 , 1 },    // 1 controlled leds in sorted order (when looking at front, from left -> to -> right)
//   { 1 , 0 },    // 2 every element is one led with
//   { 1 , 2 },    // 3 {anode-pin,cathode-pin}
//   { 0 , 1 },    // 4 "pin" means here - index in controlPins
//   { 2 , 0 },    // 5 array defined above
//   { 0 , 2 }     // 6
// };

const int dTime = 50;
int gameEnabled = 0;
int WifiFlags[] = {0,0,0,0,0,0};
int completedArray[] = {1,1,1,1,1,1};
int noFlags[] = {0,0,0,0,0,0};

enum WifiFlagValues
{
  WifiFlag1 = 1,
  WifiFlag2 = 2,
  WifiFlag3 = 4,
  WifiFlag4 = 8,
  WifiFlag5 = 16,
  WifiFlag6 = 32
}

WifiFlagArray[] = {WifiFlag1, WifiFlag2, WifiFlag3, WifiFlag4, WifiFlag5, WifiFlag6};

inline WifiFlagValues operator|(WifiFlagValues a, WifiFlagValues b)
{return static_cast<WifiFlagValues>(static_cast<int>(a) | static_cast<int>(b));}

byte eepromFlags = 0;
int flagsFound = 0;
bool gameCompleted = false;
bool scanning = false;

long goneTime;
#define NEWPATTERN 100 //100 ms for new LED pattern
os_timer_t myTimer;
bool reverseAnimation = false;

void timerCallback(void *pArg) {
  myCharlie.outRow();
} // End of timerCallback

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  os_timer_setfn(&myTimer, timerCallback, NULL);
  os_timer_arm(&myTimer, 5, true);
  randomSeed(analogRead(0));
  goneTime = millis();

  //get current EEPROM value
  eepromFlags = EEPROM.read(0);

  //restore WifiFlags based on eeprom
  for(int index = 0; index <= 5; ++index) {
    int WifiFlagValue = WifiFlagArray[index];
    //Testing if eeprom contains flag
    if ((eepromFlags & WifiFlagValue) != 0) {
      //restore saved flag
      WifiFlags[index] = 1;
      gameEnabled = 1;
    }
  }
}

void loop() {
  unsigned long now = millis(); //get current 'time'
  const unsigned long fiveMinutes = 15 * 1000UL;  // scan every 15 seconds
  static unsigned long lastSampleTime = 0 - fiveMinutes;  // initialize such that a reading is due the first time through loop()
  if (now - lastSampleTime >= fiveMinutes) {
    lastSampleTime += fiveMinutes;
    if(!gameCompleted){
     //if the game is not complete, time to scan...
     listNetworks();
    }
  }

  if(gameCompleted){
    //game is completed, show the winning animation
    showGameCompleteAnimation();
  }
  else if(scanning){
    //show scan animation is scanning is set to true
    scanAnimation();
  }
  else{
    //otherwise, show the game score
    showGameScore();
  }
}

void listNetworks() {
  // scan for nearby networks:
  scanning = true;
  //this allows the esp to scan while not blocking the scan animation
  //when scanning is complete, it calls the function 'onWifiScanCOmplete'
  WiFi.scanNetworksAsync(onWifiScanComplete, true);
}

void onWifiScanComplete(int numNetworksFound) {
  //scan complete
  String badgePirateSSID = "badgepirates";
  // iterate over each ssid, then iterate over each led to check if we should
  for (int thisNet = 0; thisNet < numNetworksFound; thisNet++) {
    String thisSSID = WiFi.SSID(thisNet); //convert the ssid to a string that we can compare on

    //in testing, we would get a buffer overflow if we iterated over too many access points
    //to prevent this, we only iterate over the SSID if the string starts with 'badgepirates'
    // the result of .indexOf(...) in this case should be 0 since it's at the start of the string
    if (thisSSID.indexOf("badgepirates") >= 0) {
      for(int ssidIndex = 0; ssidIndex <= 5; ssidIndex++) { //iterate over each of the 6 potential access points
        if( thisSSID == badgePirateSSID + ssidIndex){
          gameEnabled = 1; //this keeps the badge scan animation going until the badge finds its first flag
          WifiFlags[ssidIndex] = 1; //set the found flag to light up
          flagsFound |= WifiFlagArray[ssidIndex]; //update the flag array
          setEEPROM();
        }
      }
    }

    //setup a hotspot or AP with the SSID "badgepirates-nuke".  If a badge scans this SSID
    //it will reset the EEPROM, flag values, and lights as if it were a new badge.
    if(thisSSID == "badgepirates-nuke") {
      for(int i = 0; i <= 5; i++) {
        WifiFlags[i] = 0;
      }
      resetEEPROM();
    }
  }

  //check and set gameComplated to true if badge has found all flags
  gameCompleted = checkArrays(WifiFlags, completedArray, 6);

  //keep the scanning animation if the user has not found any flags yet
  if(!checkArrays(WifiFlags, noFlags, 6)) {
    scanning = false;
  }
}

void setEEPROM() {
  int allFlagsFound = eepromFlags | flagsFound;
  if (allFlagsFound > eepromFlags) {
    EEPROM.write(0, allFlagsFound);
    EEPROM.commit();
  }
}

void resetEEPROM() {
  //drop the nuke
  EEPROM.write(0, 0);
  EEPROM.commit();
}

//compares two arrays and returns true/false
boolean checkArrays(int arrayA[],int arrayB[], long numItems) {
  boolean same = true;
  long i = 0;
  while(i<numItems && same) {
    same = arrayA[i] == arrayB[i];
    i++;
  }
  return same;
}

/////////////////////////////////////////////////
////////////////                /////////////////
//////////////// animation code /////////////////
////////////////                /////////////////
/////////////////////////////////////////////////

void showGameCompleteAnimation(){
  //flicker pattern
  if (millis()-goneTime >= NEWPATTERN) {
    for (int i=0; i<10; i++) {
      myCharlie.ledWrite(scoreLeds[i], (byte)random(0,2));
    }
    goneTime = millis();
  }
}

void showGameScore() {

    if(WifiFlags[0] == 1){
      myCharlie.ledWrite(scoreLeds[0], ON);
    } else {
      myCharlie.ledWrite(scoreLeds[0], OFF);
    }

    if(WifiFlags[1] == 1){
      myCharlie.ledWrite(scoreLeds[1], ON);
    } else {
      myCharlie.ledWrite(scoreLeds[1], OFF);
    }

    if(WifiFlags[2] == 1){
      myCharlie.ledWrite(scoreLeds[2], ON);
    } else {
      myCharlie.ledWrite(scoreLeds[2], OFF);
    }

    if(WifiFlags[3] == 1){
      myCharlie.ledWrite(scoreLeds[3], ON);
    } else {
      myCharlie.ledWrite(scoreLeds[3], OFF);
    }

    if(WifiFlags[4] == 1){
      myCharlie.ledWrite(scoreLeds[4], ON);
    } else {
      myCharlie.ledWrite(scoreLeds[4], OFF);
    }

    if(WifiFlags[5] == 1){
      myCharlie.ledWrite(scoreLeds[5], ON);
    } else {
      myCharlie.ledWrite(scoreLeds[5], OFF);
    }
}

void scanAnimation(){
  if(!reverseAnimation) {
    for (int i=0; i < 6; i++) {
      for(int x=0; x < 6; x++){
        if(x == i) { myCharlie.ledWrite(scoreLeds[x], ON); }
        else {myCharlie.ledWrite(scoreLeds[x], OFF);}
      }
      delay(dTime);
    }
    reverseAnimation = true;
  }
  else {
    for (int i=5; i > 0; i--) {
      for(int x=5; x > 0; x--){
        if(x == i) { myCharlie.ledWrite(scoreLeds[x], ON); }
        else {myCharlie.ledWrite(scoreLeds[x], OFF);}
      }
      delay(dTime);
    }
    reverseAnimation = false;
  }
}
