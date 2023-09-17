#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiMulti wifiMulti;

void setup()
{
  Serial.begin(9600);
  delay(3000);

  wifiMulti.addAP(ssid, pass);

  Serial.println("Ready");

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop()
{
  if (wifiMulti.run() == WL_CONNECTED)
  {
    HTTPClient http;
    // http.begin("http://www.zeppelinmaker.it/helloworld.txt");
    http.begin("http://172.20.10.8:8000");
    // http.addHeader("ngrok-skip-browser-warning", "69420");

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
      else {
        Serial.println("HTTP GET: Failed");
        Serial.println(http.errorToString(httpCode).c_str());
      }
    }
    http.end();
  }

  delay(5000);

}