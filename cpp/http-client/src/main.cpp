#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// WiFi Settings ///////
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiMulti wifiMulti;

void setup()
{
  Serial.begin(9600);
  delay(3000);

  wifiMulti.addAP(ssid, pass);

  Serial.println("Ready");

  // WiFi.begin(ssid, pass);
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   delay(500);
  //   Serial.print(".");
  // }

  // // print the SSID of the network you're attached to:
  // Serial.print("SSID: ");
  // Serial.println(WiFi.SSID());

  // // print your WiFi shield's IP address:
  // IPAddress ip = WiFi.localIP();
  // Serial.print("IP Address: ");
  // Serial.println(ip);
}

void loop()
{
  if (wifiMulti.run() == WL_CONNECTED)
  {
    HTTPClient http;
    // http.begin("http://www.zeppelinmaker.it/helloworld.txt");
    http.begin("http://84d0-2-37-165-196.ngrok-free.app");
    // http.begin(serverName.c_str());
    http.addHeader("ngrok-skip-browser-warning", "69420");

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

  // Serial.println("making GET request");
  // client.get("/");

  // // read the status code and body of the response
  // int statusCode = client.responseStatusCode();
  // String response = client.responseBody();

  // Serial.print("Status code: ");
  // Serial.println(statusCode);
  // Serial.print("Response: ");
  // Serial.println(response);
  // Serial.println("Wait five seconds");
  // delay(5000);
}









// #include <Arduino.h>
// #include <WiFiClientSecure.h>

// const char *ssid = "VodafoneUnSaccoDiNumeri";
// const char *password = "Gio2810Richipure";

// const char *server = "https://bdd1-2-37-165-196.ngrok-free.app";

// WiFiClientSecure client;

// void setup()
// {
//   Serial.begin(9600);
//   delay(3000);

//   Serial.println("Ready");

//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("Connected");

//   client.setInsecure();

//   if (!client.connect(server, 80))
//     Serial.println("Connection failed!");
//   else
//   {
//     Serial.println("Connected to server!");
//     // Make a HTTP request:
//     client.println("GET https://bdd1-2-37-165-196.ngrok-free.app HTTP/1.0");
//     client.println("Host: bdd1-2-37-165-196.ngrok-free.app");
//     client.println("Connection: close");
//     client.println();
//   }

//   while (client.connected())
//   {
//     String line = client.readStringUntil('\n');
//     if (line == "\r")
//     {
//       Serial.println("headers received");
//       break;
//     }
//   }
//   // if there are incoming bytes available
//   // from the server, read them and print them:
//   while (client.available())
//   {
//     char c = client.read();
//     Serial.write(c);
//   }

//   client.stop();
// }

// void loop()
// {
//   // do nothing
// }
