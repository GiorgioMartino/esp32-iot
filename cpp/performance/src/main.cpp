#include <Arduino.h>
#include <BluetoothSerial.h>
#include <chrono>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include "secrets.h"

using namespace std::chrono;

// WIFI Credentials
char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;

// Connections
IPAddress ip(192, 168, 1, 12);
// IPAddress ip(172, 20, 10, 8);
WiFiClient wifiClient;
PubSubClient mqttClient(ip, 1883, wifiClient);
BluetoothSerial SerialBT;
WiFiUDP udp;
Coap coap(udp);

const int STATS_TARGET = 10;
const String MESSAGE_CONTENT = "Hello From ESP32";

// Global variables
int bltSentCount;
int coapSentCount;
int httpSentCount;
int bltReceivedCount;
int coapReceivedCount;
int httpReceivedCount;
long bltTotalTime;
long coapTotalTime;
long httpTotalTime;
long bltSentTime;
long bltReceivedTime;
long coapSentTime;
long coapReceivedTime;
long httpSentTime;
long httpReceivedTime;

boolean coapSent = false;
boolean httpSent = false;

int packetLoss;
double latency;
double bytes;
double throughput;

// Functions definitions
void resetStats(int type);
void callback_response(CoapPacket &packet, IPAddress ip, int port);

void setup()
{
  // Setup
  Serial.begin(9600);
  delay(3000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Setup Bluetooth
  SerialBT.begin("ESP32-Giorgio");
  Serial.println("The device started, now you can pair it with bluetooth!");

  // Setup MQTT
  mqttClient.setServer(ip, 1883);
  while (!mqttClient.connected())
  {
    if (mqttClient.connect("ESP32"))
    {
      Serial.println("Connected to broker MQTT");
    }
    else
    {
      Serial.print(".");
      delay(1000);
    }
  }
  mqttClient.loop();

  // CoAP
  coap.response(callback_response);
  coap.start();

  // Initialize stats
  resetStats(0);
}

void loop()
{
  // Bluetooth
  if (bltSentCount < STATS_TARGET)
  {
    Serial.println("Sending bluetooth");
    bltSentTime = millis();
    SerialBT.println("Hello From ESP32");
    bltSentCount++;
    String bltResponse = SerialBT.readString();
    if (bltResponse != NULL)
    {
      Serial.println("Received bluetooth");
      bltReceivedTime = millis();
      bltTotalTime += (bltReceivedTime - bltSentTime);
      bltReceivedCount++;
    }
  }
  else
  {
    try
    {
      // Publish Bluetooth results on Node-RED via MQTT
      packetLoss = STATS_TARGET - bltReceivedCount;
      latency = bltTotalTime / bltReceivedCount;
      bytes = MESSAGE_CONTENT.length() * sizeof(wchar_t) * STATS_TARGET;
      throughput = bytes / (bltTotalTime / 1000);

      mqttClient.publish("bluetooth/latency", String(latency).c_str());
      mqttClient.publish("bluetooth/packetLoss", String(packetLoss).c_str());
      mqttClient.publish("bluetooth/throughput", String(throughput).c_str());
    }
    catch (...)
    {
      Serial.println("ERROR");
    }
    resetStats(1);
  }

  // CoAP
  if (coapSentCount < STATS_TARGET)
  {
    if (!coapSent)
    {
      coapSentTime = millis();
      Serial.println("Sending CoAP");
      coap.put(ip, 5683, "hello", "Hello From ESP32");
      coapSent = true;
      coapSentCount++;
    }
    coap.loop();
  }
  else
  {
    try
    {
      // Publish CoAP results on Node-RED via MQTT
      packetLoss = STATS_TARGET - coapReceivedCount;
      latency = coapTotalTime / coapReceivedCount;
      bytes = MESSAGE_CONTENT.length() * sizeof(wchar_t) * STATS_TARGET;
      throughput = bytes / (coapTotalTime / 1000);

      mqttClient.publish("coap/latency", String(latency).c_str());
      mqttClient.publish("coap/packetLoss", String(packetLoss).c_str());
      mqttClient.publish("coap/throughput", String(throughput).c_str());
    }
    catch (...)
    {
      Serial.println("ERROR");
    }
    resetStats(2);
  }
}

void callback_response(CoapPacket &packet, IPAddress ip, int port)
{
  Serial.println("[Coap Response got]");

  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  // Serial.println(p);

  coapReceivedTime = millis();
  coapTotalTime += (coapReceivedTime - coapSentTime);
  coapReceivedCount++;
  Serial.print("Total time: ");
  Serial.println(coapTotalTime);
  Serial.print("Received count: ");
  Serial.println(coapReceivedCount);

  coapSent = false;
}

void resetStats(int type)
{
  if (type == 0)
  {
    bltSentCount = 0;
    coapSentCount = 0;
    httpSentCount = 0;
    bltReceivedCount = 0;
    coapReceivedCount = 0;
    httpReceivedCount = 0;
    bltTotalTime = 0;
    coapTotalTime = 0;
    httpTotalTime = 0;
  }
  if (type == 1)
  {
    bltSentCount = 0;
    bltReceivedCount = 0;
    bltTotalTime = 0;
  }
  if (type == 2)
  {
    coapSentCount = 0;
    coapReceivedCount = 0;
    coapTotalTime = 0;
  }
  if (type == 3)
  {
    httpSentCount = 0;
    httpReceivedCount = 0;
    httpTotalTime = 0;
  }
}
