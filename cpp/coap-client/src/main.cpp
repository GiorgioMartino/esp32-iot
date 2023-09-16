#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include "pw_secret.h"

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port);

// CoAP server endpoint url callback
void callback_light(CoapPacket &packet, IPAddress ip, int port);

// UDP and CoAP class
WiFiUDP udp;
Coap coap(udp);

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port)
{
  Serial.println("[Coap Response got]");

  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  Serial.println(p);
}

void callback_light(CoapPacket &packet, IPAddress ip, int port)
{
  // This is a callback function that simulates the control lamp by receiving the command
  Serial.print("Received from: ");
  Serial.println(ip);
  Serial.println(packet.messageid);

  // Send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  coap.sendResponse(ip, port, packet.messageid, "1");
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(SSID, PW);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Setup Callback Light");
  // In fact, the registration server handles the callback function
  // Add the handler pointer and url to uri.add
  coap.server(callback_light, "light");

  // client response callback.
  // this endpoint is single callback.
  Serial.println("Setup Response Callback");
  coap.response(callback_response);

  // start coap server/client
  coap.start();
  coap.get(IPAddress(192, 168, 1, 12), 5683, "time");
}

void loop()
{
  coap.get(IPAddress(192, 168, 1, 12), 5683, "time");

  delay(1000);
  coap.loop();
  coap.get(IPAddress(192, 168, 1, 12), 5683, "time");
}