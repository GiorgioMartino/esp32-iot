#include <Arduino.h>
#include <BluetoothSerial.h>
#include <chrono>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

using namespace std::chrono;

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;

IPAddress ip(192, 168, 1, 12);

WiFiClient espClient;
PubSubClient client(ip, 1883, espClient);

BluetoothSerial SerialBT;

int msgSent = 0;

long ms;
long receivedMs;

int count = 0;
double totalRTT = 0;
double avg = 0;
int packetLoss = 0;

double throughput = 0;
long initalMs = millis();

bool start = false;

void setup()
{
  // Setup
  Serial.begin(9600);
  delay(3000);
  SerialBT.begin("ESP32-Giorgio");
  Serial.println("The device started, now you can pair it with bluetooth!");

  // Setup MQTT
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  client.setServer(ip, 1883);
  while (!client.connected())
  {
    if (client.connect("ESP32"))
    {
      Serial.println("Connected to broker MQTT");
    }
    else
    {
      Serial.print(".");
      delay(1000);
    }
  }

  client.loop();
}

void loop()
{
  // Send data through Bluetooth socket
  ms = millis();
  SerialBT.println("Hello From ESP32");
  msgSent++;

  // Receive data through Bluetooth socket
  String pythonJson = SerialBT.readString();
  if (pythonJson != NULL)
  {
    start = true;
    // Serial.println("Received");
    // Serial.println(pythonJson);

    receivedMs = millis();
    double rtt = (receivedMs - ms) / 2;
    // Serial.println(rtt, 4);
    count++;
    totalRTT += rtt;
  }
  else
  {
    if (start)
    {
      packetLoss++;
    }
  }

  if (msgSent == 10)
  {
    avg = totalRTT / count;
    Serial.println("AVG RTT");
    Serial.println(avg, 4);
    Serial.println("Packet Loss");
    Serial.println(packetLoss, 10);
    double bytes_sent = sizeof("Hello From ESP32") * msgSent;
    throughput = bytes_sent / (millis() - initalMs) * 1000;
    Serial.println("Throughput");
    Serial.println(throughput, 4);

    msgSent = 0;
    totalRTT = 0;
    count = 0;
    initalMs = millis();
    packetLoss = 0;

    client.publish("topic/bluetooth/rtt", String(avg).c_str());
    client.publish("topic/bluetooth/packetLoss", String(packetLoss).c_str());
    client.publish("topic/bluetooth/throughput", String(throughput).c_str());
  }

  // delay(1000);
}
