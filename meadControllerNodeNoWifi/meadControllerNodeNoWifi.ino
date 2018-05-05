#include "Adafruit_MCP9808.h"
#include <ESP8266WiFi.h>

const int relay = 14;   //NodeMCU 1.0 Pin D5
const float tempHigh = 25;  //Celcius
const float tempLow = 20;   //Celcius

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

enum state
{
  off = 0,
  on = 1
};

state fridgeState = off;

void setup()
{
  tempsensor.begin();
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  WiFi.forceSleepBegin();
}

void loop()
{
  tempsensor.wake();   // wake up, ready to read!
  float c = tempsensor.readTempC();

  switch (fridgeState)
  {
    case on:
    {
      if (c < tempLow)
      {
        fridgeState = off;
        digitalWrite(relay, LOW);
        tempsensor.shutdown(); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
      }
      break;
    }
    case off:
    {
      if (c > tempHigh)
      {
        fridgeState = on;
        digitalWrite(relay, HIGH);
      }
      tempsensor.shutdown(); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
      break;
    }
    default:
    {
      fridgeState = off;
      tempsensor.shutdown(); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
      digitalWrite(relay, LOW);
    }
  }
  delay(5000);
}
