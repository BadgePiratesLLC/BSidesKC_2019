#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <Adafruit_NeoPixel.h>
#include "pitches.h"
#include "babysharkToneMelody.h"

#define PIN      6
#define N_LEDS 144

const int IRrxPin = 5;
const int IRtxPin = 13;
const int pewpewPin = 4;  //pewpew button pin
const int speakerPin = 12;  //piezo speaker
const int rgbledPin = 14;  //rgbleds
const int rgbledNum = 3;  // number of rgb leds
const int vibratingDiskPin = 15;
volatile byte pewpewCounter = 0;

bool gameEnabled = false;

unsigned long rxIRCode;
int pewpewPressed = 0;
int gameScore = 3;
int gameScoreLights[] = {1,1,1};

//IRREmoteESP8266 definitions
IRrecv irrecv(IRrxPin);  // setup RX Pin
IRsend irsend(IRtxPin);  // setup TX Pin
decode_results results;  // Start RX


uint16_t txpewpewCode[67] = {9000, 4500, 650, 550, 650, 1650, 600, 550, 650, 550,
                        600, 1650, 650, 550, 600, 1650, 650, 1650, 650, 1650,
                        600, 550, 650, 1650, 650, 1650, 650, 550, 600, 1650,
                        650, 1650, 650, 550, 650, 550, 650, 1650, 650, 550,
                        650, 550, 650, 550, 600, 550, 650, 550, 650, 550,
                        650, 1650, 600, 550, 650, 1650, 650, 1650, 650, 1650,
                        650, 1650, 650, 1650, 650, 1650, 600};


// Setup Neopixel LEDs
Adafruit_NeoPixel strip = Adafruit_NeoPixel(rgbledNum, rgbledPin, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  playBabySharkSong();
  // Setup interupt for PewPew button
  pinMode(pewpewPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pewpewPin), pewpewInterrupt, FALLING);

  // Setup Piezo Speaker
  pinMode(speakerPin, OUTPUT);

  // Setup Vibrating Disk
  pinMode(vibratingDiskPin, OUTPUT);

  // Setup Neopixel
   strip.begin();

// Setup IR Monitor
    Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(IRrxPin);

  irsend.begin();  // Start TX
  irrecv.enableIRIn();  // Start the receiver

  Serial.println("Ready to do battle!");

}

void pewpewInterrupt() {
  gameEnabled = true;
  //Report on the number of shots fired
  Serial.println("PewPew Button Pushed");
  pewpewCounter++;  //increment counter

  Serial.println();
  Serial.print("Number of shots fired: ");
  Serial.print(pewpewCounter);
  Serial.println();

  //Begin Firing Sequence

  Serial.println("I'm a firing mah lazar");
  irsend.sendRaw(txpewpewCode, 67, 38);  // Send a raw data capture at 38kHz.
  pewpewPressed++;

}


static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}


void loop() {
  // put your main code here, to run repeatedly:

// check to see if we should play the pewpew sound

/*
if (pewpewPressed <= 0) { // check that we didn't accidently get below 0 somehow
  pewpewPressed = 0;
  }
  else {  // we need to play the sound
  tone(speakerPin, 9000); // Send 1KHz sound signal...
  delay(250);        // ...for 1 sec
  noTone(speakerPin);     // Stop sound...
  pewpewPressed--; //back to zero...hopefully
  }
 */


  if(!gameEnabled){
    chase(strip.Color(255, 0, 0)); // Red
    chase(strip.Color(0, 255, 0)); // Green
    chase(strip.Color(0, 0, 255)); // Blue
  } else {
    displayScore();
    if (irrecv.decode(&results)) {
      // print() & println() can't handle printing long longs. (uint64_t)
      Serial.print("Hex IRcode: ");
      serialPrintUint64(results.value, HEX);
      //serialPrintUint64(results.value);
      Serial.println("");

      rxIRCode = results.value;
      Serial.print("rxIRCode value: ");
      Serial.println(rxIRCode);
      if (rxIRCode == 1270235327) {
        Serial.println("I'm hit!");
        tone(speakerPin, 1000); // Send 1KHz sound signal...
        digitalWrite(vibratingDiskPin, HIGH);  // buzz
        delay(1000);        // ...for 1 sec
        noTone(speakerPin);     // Stop sound..
        digitalWrite(vibratingDiskPin, LOW);  // stop buzzing
        decrementScore();
      }

      if (rxIRCode == 0x4BB640BF) {
        Serial.println("Actually I'm here");
      }

      irrecv.resume();  // Receive the next value
    }
  }
}

void displayScore(){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(gameScoreLights[i] == 1){
      strip.setPixelColor(i  , strip.Color(0, 0, 255)); // Draw new pixel
    }
    else {
      strip.setPixelColor(i  , strip.Color(0, 0, 0)); // Draw new pixel
    }
    strip.show();
  }
}

void decrementScore(){
  gameScore--;
  gameScoreLights[gameScore] = 0;

  if(gameScore <= 0){
    gameScore = 3;
    for(int i=0; i<=2; i++){
      gameScoreLights[i] = 1;
    }
  }
}

void playBabySharkSong(){
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 30; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speakerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speakerPin);
  }
}
