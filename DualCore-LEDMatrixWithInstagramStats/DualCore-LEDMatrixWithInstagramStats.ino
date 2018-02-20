/*******************************************************************
 *  Getting stats from Instagram and using the second core
 *  of the ESP32 to drive a MAX7219 LED Matrix
 *
 *  Written by Brian Lough
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

 // ----------------------------
 // Standard Libraries - Already Installed if you have ESP32 set up
 // ----------------------------

#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>


// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include "LedMatrix.h"
// The driver for the LED Matrix Display
// Using my fork of Daniel Eichorn's library (For support for software SPI and rotation)
// https://github.com/witnessmenow/MAX7219LedMatrix

#include "InstagramStats.h"
// Fetches follower count from Instagram
// Should be available on the library manager (search for "Instagram"), if not download from Github
// https://github.com/witnessmenow/arduino-instagram-stats

#include "JsonStreamingParser.h"
// Required by the Instagram library, used for parsing large JSON payloads
// Available on the library manager (search for "Json Streaming Parser" by Daniel Eichorn)
// https://github.com/squix78/json-streaming-parser

//------- Replace the following! ------
char ssid[] = "SSID";       // your network SSID (name)
char password[] = "password";  // your network key

//Inputs
String userName = "brian_lough"; // from their instagram url https://www.instagram.com/brian_lough/

// ------ Instagram ------ 
WiFiClientSecure client;
InstagramStats instaStats(client);

unsigned long delayBetweenChecks = 60000; //mean time between api requests
unsigned long whenDueToCheck = 0;

// ------ LED Matrix ------ 

#define NUMBER_OF_DEVICES 4

// Wiring that works with ESP32
#define CS_PIN 15
#define CLK_PIN 14
#define MISO_PIN 2 //we do not use this pin just fill to match constructor
#define MOSI_PIN 12

LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

bool newData = false;
int instagramFollowers = 0;

TaskHandle_t Task1;

void driveDisplay(void * parameter){
  while(true){
    ledMatrix.clear();
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(25);
  
    if (newData) {
      newData = false;
      ledMatrix.setNextText("Followers: " + String(instagramFollowers));
    }
  }
}

void setup() {

  Serial.begin(115200);
  
  ledMatrix.init();
  ledMatrix.setRotation(true);
  ledMatrix.setText("Instagram Display");
  ledMatrix.setIntensity(8);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  xTaskCreatePinnedToCore(
    driveDisplay,            /* Task function. */
    "DisplayDrive",                 /* name of task. */
    1000,                    /* Stack size of task */
    NULL,                     /* parameter of the task */
    1,                        /* priority of the task */
    &Task1,                   /* Task handle to keep track of created task */
    0);                       /* Core */ 
}

void getInstagramStatsForUser() {
  Serial.println("Getting instagram user stats for " + userName );
  InstagramUserStats response = instaStats.getUserStats(userName);
  Serial.println("Response:");
  Serial.print("Number of followers: ");
  Serial.println(response.followedByCount);

  newData = true;
  instagramFollowers = response.followedByCount;
}

void loop() {
  unsigned long timeNow = millis();
  if ((timeNow > whenDueToCheck))  {
    getInstagramStatsForUser();
    whenDueToCheck = timeNow + delayBetweenChecks;
  }
}
