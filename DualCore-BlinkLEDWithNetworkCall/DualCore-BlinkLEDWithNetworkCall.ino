/*********************************************************************
 *  Blinking LED an one core while making network requests on the                                          
 *  other using the ESP32
 *                                                                   
 *  By Brian Lough                                                   
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA         
 *********************************************************************/

#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "InstagramStats.h"
#include "JsonStreamingParser.h"

//------- Replace the following! ------
char ssid[] = "SSID";       // your network SSID (name)
char password[] = "Password";  // your network key

String userName = "brian_lough"; // from their instagram url https://www.instagram.com/brian_lough/

WiFiClientSecure client;
InstagramStats instaStats(client);

#define GREEN_PIN 33
#define RED_PIN 34

unsigned long lastRedTime;
unsigned long lastGreenTime;

bool greenLedOn = false;
bool redLedOn = false;

TaskHandle_t Task1;

void blinkGreenLed(void * parameter){
  while(true) {
    greenLedOn = !greenLedOn;
    digitalWrite(GREEN_PIN, greenLedOn);
    delay(500);
  }
}

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  Serial.begin(115200);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

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
    blinkGreenLed,            /* Task function. */
    "blinkGreenLED",                 /* name of task. */
    1000,                    /* Stack size of task */
    NULL,                     /* parameter of the task */
    1,                        /* priority of the task */
    &Task1,                   /* Task handle to keep track of created task */
    0);                       /* Core */ 
}

void loop() {
  // millis returns number of milliseconds since device started up
  unsigned long now = millis();

  // Red changes every 1 second
  if(now - lastRedTime > 1000)
  {
    instaStats.getUserStats(userName);
    lastRedTime = now;
  }

  // Green Changes every 0.5 seconds
//  if(now - lastGreenTime > 500)
//  {
//    greenLedOn = !greenLedOn;
//    digitalWrite(GREEN_PIN, greenLedOn);
//    lastGreenTime = now;
//  }
}
