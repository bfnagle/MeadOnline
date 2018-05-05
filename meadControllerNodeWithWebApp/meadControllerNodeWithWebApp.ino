#include "Adafruit_MCP9808.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const int relay = 14;   //NodeMCU 1.0 Pin D5
const float tempHigh = 25;  //Celcius
const float tempLow = 20;   //Celcius

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

const char* ssid     = "WifiName";
const char* password = "Password";

ESP8266WiFiMulti WiFiMulti;

const char* host = "https://yourwebsite.com";
const char* fingerprint = "hex key";
const int port = 5000;

const char* batchName = "code_test";

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();


enum state
{
  off = 0,
  on = 1
};

state fridgeState = off;

int writeCounter = 0;

void setup()
{
  tempsensor.begin();
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
  }
  root["batch"] = batchName;
}

void loop()
{
  tempsensor.wake();   // wake up, ready to read!
  float c = tempsensor.readTempC();
  float f = (c * 1.8) + 32;

  root["temp"] = int(f);

  String serializedJSON;
  root.printTo(serializedJSON);

  //Only write once every 5 minutes
  if (writeCounter == 0)
  {
    if (WiFiMulti.run() == WL_CONNECTED)
    {
      HTTPClient http;
      http.begin(host, fingerprint);

      http.addHeader("Content-Type", "application/json");  //Specify content-type header
      int httpCode = http.PUT(serializedJSON);

      http.end();
    }
  }

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

  writeCounter++;
  writeCounter %= 60;

  delay(5000);
}

