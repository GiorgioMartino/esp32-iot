#include <Arduino.h>
#include <BluetoothSerial.h>
#include <chrono>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include <HTTPClient.h>
#include <BLEDevice.h>
#include "secrets.h"

using namespace std::chrono;

// WIFI Credentials
char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;

// Connections
IPAddress ip(192, 168, 1, 12);
const String httpHost = "http://192.168.1.12:8000";
// IPAddress ip(172, 20, 10, 8);
// const String httpHost = "http://172.20.10.8:8000";
WiFiClient wifiClient;
PubSubClient mqttClient(ip, 1883, wifiClient);
BluetoothSerial SerialBT;
WiFiUDP udp;
Coap coap(udp);
HTTPClient httpClient;

const int STATS_TARGET = 5;
const char jsonPayload[] = "{\"data\":\"Hello from ESP32\"}";

// Global variables
int bltSentCount;
int coapSentCount;
int httpSentCount;
int bltReceivedCount;
int coapReceivedCount;
int httpReceivedCount;
double bltTotalTime;
double coapTotalTime;
double httpTotalTime;
double bltSentTime;
double bltReceivedTime;
double coapSentTime;
double coapReceivedTime;
double httpSentTime;
double httpReceivedTime;

boolean coapSent = false;

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
  Serial.print("TX power:");
  Serial.println(esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_DEFAULT));
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
  Serial.print("TX power updated:");
  Serial.println(esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_DEFAULT));

  // Setup Bluetooth
  SerialBT.begin("ESP32-Giorgio");
  Serial.println("The device started, now you can pair it with bluetooth!");

  // Setup WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

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

  // Setup CoAP
  coap.response(callback_response);
  coap.start();

  // Setup HTTP
  httpClient.begin(httpHost);

  // Initialize stats
  resetStats(0);
  delay(10000);
  Serial.println("SETUP Completed");
}

void loop()
{
  // Bluetooth
  if (bltSentCount < STATS_TARGET)
  {
    // Serial.println("Sending bluetooth");
    bltSentTime = millis();
    SerialBT.println(jsonPayload);
    bltSentCount++;
    String bltResponse = SerialBT.readString();
    if (bltResponse != NULL)
    {
      // Serial.println("Received bluetooth");
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
      bytes = sizeof(jsonPayload) * STATS_TARGET;
      throughput = bytes / (bltTotalTime / 1000);

      mqttClient.publish("bluetooth/latency", String(latency).c_str());
      mqttClient.publish("bluetooth/packetLoss", String(packetLoss).c_str());
      mqttClient.publish("bluetooth/throughput", String(throughput).c_str());
      // Serial.println("Published Bluetooth results");
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
      Serial.print(" -----> Sending CoAP at: ");
      Serial.println(coapSentTime);
      coap.put(ip, 5683, "hello", jsonPayload);
      coapSent = true;
      coapSentCount++;
    }
    else
    {
      double time = millis();
      if ((time - coapSentTime) > 3000)
      {
        Serial.println("Packet lost");
        Serial.print(time);
        Serial.print(" - ");
        Serial.println(coapSentTime);
        coapSent = false;
      }
      else
      {
        Serial.println("Maybe not lost");
      }
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
      bytes = sizeof(jsonPayload) * STATS_TARGET;
      throughput = bytes / (coapTotalTime / 1000);

      mqttClient.publish("coap/latency", String(latency).c_str());
      mqttClient.publish("coap/packetLoss", String(packetLoss).c_str());
      mqttClient.publish("coap/throughput", String(throughput).c_str());
      // Serial.println("Published CoAP results");
    }
    catch (...)
    {
      Serial.println("ERROR");
    }
    resetStats(2);
  }

  // HTTP
  if (httpSentCount < STATS_TARGET)
  {
    httpSentTime = millis();
    // Serial.println("Sending HTTP");
    int httpCode = httpClient.POST(jsonPayload);
    httpSentCount++;
    if (httpCode == HTTP_CODE_OK)
    {
      // Serial.println("Received HTTP Response");
      httpReceivedTime = millis();
      httpTotalTime += (httpReceivedTime - httpSentTime);
      httpReceivedCount++;
    }
  }
  else
  {
    try
    {
      // Publish HTTP results on Node-RED via MQTT
      packetLoss = STATS_TARGET - httpReceivedCount;
      latency = httpTotalTime / httpReceivedCount;
      bytes = sizeof(jsonPayload) * STATS_TARGET;
      throughput = bytes / (httpTotalTime / 1000);

      mqttClient.publish("http/latency", String(latency).c_str());
      mqttClient.publish("http/packetLoss", String(packetLoss).c_str());
      mqttClient.publish("http/throughput", String(throughput).c_str());
      // Serial.println("Published HTTP results");
    }
    catch (...)
    {
      Serial.println("ERROR");
    }
    resetStats(3);
  }
}

void callback_response(CoapPacket &packet, IPAddress ip, int port)
{
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  // Serial.println(p);

  coapReceivedTime = millis();
  coapTotalTime += (coapReceivedTime - coapSentTime);
  coapReceivedCount++;

  Serial.print(" <----- Received CoAP Response at: ");
  Serial.println(coapReceivedTime);

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
