/*********************************************************************
 *  Blink 2 LEDS at the same time                                           
 *  I know, amazing right?
 *                                                                   
 *  By Brian Lough                                                   
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA         
 *********************************************************************/


#define GREEN_PIN D5
#define RED_PIN D6

unsigned long lastRedTime;
unsigned long lastGreenTime;

bool greenLedOn = false;
bool redLedOn = false;

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
}

void loop() {
  // millis returns number of milliseconds since device started up
  unsigned long now = millis();

  // Red changes every 1 second
  if(now - lastRedTime > 1000)
  {
    redLedOn = !redLedOn;
    digitalWrite(RED_PIN, redLedOn);
    lastRedTime = now;
  }

  // Green Changes every 0.5 seconds
  if(now - lastGreenTime > 500)
  {
    greenLedOn = !greenLedOn;
    digitalWrite(GREEN_PIN, greenLedOn);
    lastGreenTime = now;
  }
}
