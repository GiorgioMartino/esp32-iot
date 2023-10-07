#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiMulti wifiMulti;
HTTPClient http;
WiFiClient wifiClient;

void setup()
{
  Serial.begin(9600);
  delay(3000);

  // wifiMulti.addAP(ssid, pass);
  // while (wifiMulti.run() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  // }

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Ready");

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // http.begin("http://www.zeppelinmaker.it/helloworld.txt");
  http.begin("http://192.168.1.12:8000");
}

void loop()
{
  // if (wifiMulti.run() == WL_CONNECTED)
  // {
  // HTTPClient http;
  // http.begin("http://192.168.1.12:8000");

  Serial.println("Sending GET Request");

  int httpCode = http.GET();
  Serial.print("HTTP code:");
  Serial.println(httpCode);

  if (httpCode > 0)
  {
    if (httpCode == HTTP_CODE_OK)
    {
      String page = http.getString();
      Serial.println(page);
    }
    else
    {
      Serial.println("HTTP GET: Failed");
      Serial.println(http.errorToString(httpCode).c_str());
    }
  }

  Serial.println("Sending POST Request");

  const int capacity = JSON_OBJECT_SIZE(8);
  StaticJsonDocument<capacity> doc;
  doc["data"] = "Hello from ESP32";
  String payload;

  serializeJson(doc, payload);
  Serial.println(payload);

  // char json[] = "{\"data\":\"Hello from ESP32\"}";
  // Serial.println(json);

  int httpCode = http.POST(payload);
  Serial.print("HTTP code:");
  Serial.println(httpCode);

  if (httpCode > 0)
  {
    if (httpCode == HTTP_CODE_OK)
    {
      String page = http.getString();
      Serial.println(page);
    }
    else
    {
      Serial.println("HTTP POST: Failed");
      Serial.println(http.errorToString(httpCode).c_str());
    }
  }

  // http.end();
  // }

  delay(1000);
}