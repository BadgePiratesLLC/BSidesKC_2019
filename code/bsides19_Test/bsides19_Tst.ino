
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
const int LED_1 = 13;     //LED row 1
const int LED_2 = 12;     //LED row 2    
const int LED_3 = 10;     //LED row 3
const char *ssid = "BadgePiratesAP_01"; // The name of the Wi-Fi network that will be created
const char *password = "thereisnospoon";   // The password required to connect to it, leave blank for an open network
const int dTime = 50;


void setup() {
  Serial.begin(74880);
  delay(10);

  Serial.println();


  //WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" setup but not started");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the Monitor

  Serial.print("MAC:\t ");
  Serial.println(WiFi.macAddress());  // Send the MAC address of the ESP8266 to the Monitor
  Serial.println('\n');
  }

void loop()
{
    //turn on LED L1
  pinMode(LED_1, INPUT);      //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);     //row 2
  digitalWrite(LED_2, LOW);  
  pinMode(LED_3, OUTPUT);     //row 3
  digitalWrite(LED_3, HIGH);
  
  delay(dTime);
  
    //turn on LED L2
  pinMode(LED_1, OUTPUT);     //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);     //row 2
  digitalWrite(LED_2, HIGH);  
  pinMode(LED_3, INPUT);      //row 3
  digitalWrite(LED_3, LOW);
  
  delay(dTime);
  
  //turn on LED L3
  pinMode(LED_1, INPUT);     //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);    //row 2
  digitalWrite(LED_2, HIGH);  
  pinMode(LED_3, OUTPUT);    //row 3
  digitalWrite(LED_3, LOW);
  
  delay(dTime);
  
   //turn on LED L4
  pinMode(LED_1, OUTPUT);     //row 1
  digitalWrite(LED_1, HIGH);
  pinMode(LED_2, OUTPUT);     //row 2
  digitalWrite(LED_2, LOW);   
  pinMode(LED_3, INPUT);      //row 3
  digitalWrite(LED_3, LOW);
  
  delay(dTime);
 
  //turn on LED L5
  pinMode(LED_1, OUTPUT);    //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, INPUT);     //row 2
  digitalWrite(LED_2, LOW);
  pinMode(LED_3, OUTPUT);    //row3
  digitalWrite(LED_3, HIGH);
  
  delay(dTime);

  //turn on LED L6
  pinMode(LED_1, OUTPUT);
  digitalWrite(LED_1, HIGH);
  pinMode(LED_2, INPUT);
  digitalWrite(LED_2, LOW);
  pinMode(LED_3, OUTPUT);
  digitalWrite(LED_3, LOW);
  
  delay(dTime);

    //turn on LED L5
  pinMode(LED_1, OUTPUT);    //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, INPUT);     //row 2
  digitalWrite(LED_2, LOW);
  pinMode(LED_3, OUTPUT);    //row3
  digitalWrite(LED_3, HIGH);
  
  delay(dTime);

     //turn on LED L4
  pinMode(LED_1, OUTPUT);     //row 1
  digitalWrite(LED_1, HIGH);
  pinMode(LED_2, OUTPUT);     //row 2
  digitalWrite(LED_2, LOW);   
  pinMode(LED_3, INPUT);      //row 3
  digitalWrite(LED_3, LOW);
  
  delay(dTime);
    //turn on LED L3
  pinMode(LED_1, INPUT);     //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);    //row 2
  digitalWrite(LED_2, HIGH);  
  pinMode(LED_3, OUTPUT);    //row 3
  digitalWrite(LED_3, LOW);
  
  delay(dTime);

      //turn on LED L2
  pinMode(LED_1, OUTPUT);     //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);     //row 2
  digitalWrite(LED_2, HIGH);  
  pinMode(LED_3, INPUT);      //row 3
  digitalWrite(LED_3, LOW);

  delay(dTime);
      //turn on LED L1
  pinMode(LED_1, INPUT);      //row 1
  digitalWrite(LED_1, LOW);
  pinMode(LED_2, OUTPUT);     //row 2
  digitalWrite(LED_2, LOW);  
  pinMode(LED_3, OUTPUT);     //row 3
  digitalWrite(LED_3, HIGH);

}
