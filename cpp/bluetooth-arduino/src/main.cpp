#include <Arduino.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>
#include <chrono>

using namespace std::chrono;

// JSON example
// {
//   "message_number" : 1,
//   "start_timestamp" : 12345667,
//   "content" : "Hello from ESP32"
// }

BluetoothSerial SerialBT;

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("ESP32-Giorgio");
  Serial.println("The device started, now you can pair it with bluetooth!");
}

int msgNumber = 1;
long ms;
long receivedMs;

int count = 0;
double totalRTT = 0;
double avg = 0;
int packetLoss = 0;

bool start = false;

void loop()
{
  // Send data through Bluetooth socket
  const int capacity = JSON_OBJECT_SIZE(8);
  StaticJsonDocument<capacity> json;

  ms = millis();

  json["message_number"] = msgNumber;
  json["current_timestamp"] = ms;
  json["content"] = "Hello from ESP32";

  std::string normalJson;
  serializeJson(json, normalJson);

  uint8_t buf[normalJson.length()];
  memcpy(buf, normalJson.c_str(), normalJson.length());
  SerialBT.write(buf, normalJson.length());

  Serial.println("Sent");
  Serial.write(buf, normalJson.length());
  Serial.println();

  // Receive data through Bluetooth socket

  String pythonJson = SerialBT.readString();
  if (pythonJson != NULL)
  {
    start = true;
    Serial.println("Received");
    Serial.println(pythonJson);

    StaticJsonDocument<capacity> receivedJson;
    deserializeJson(receivedJson, pythonJson);

    if (receivedJson["message_number"] == msgNumber)
    {
      receivedMs = millis();

      double rtt = (receivedMs - ms) / 2;

      Serial.print("RTT: ");
      Serial.println(rtt, 4);

      count++;
      totalRTT += rtt;
    }
    else
    {
      packetLoss++;
    }

    msgNumber++;
  }
  else
  {
    if (start)
    {
      packetLoss++;
      msgNumber++;
    }
  }

  if (msgNumber == 100)
  {
    avg = totalRTT / count;
    Serial.print("AVG RTT: ");
    Serial.println(avg, 4);
    Serial.print("Packet Loss: ");
    Serial.println(packetLoss, 10);
  }

  Serial.println();

  // delay(1000);
}
