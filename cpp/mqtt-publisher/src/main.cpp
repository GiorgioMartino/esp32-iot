#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;
IPAddress ip(192, 168, 1, 12);

WiFiClient espClient;
PubSubClient client(ip, 1883, espClient);

void setup()
{
  Serial.begin(9600);
  WiFi.disconnect();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Now connected");
  client.setServer(ip, 1883);

  Serial.print("Connecting to MQTT broker.");
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

  Serial.println("client.loop()");
  client.loop();
}

void loop()
{
  Serial.println("Publishing on topic");
  double avg = 10.5;

  client.publish("topic/hello", "hello world from ESP32");
  client.publish("topic/bluetooth/rtt", String(avg).c_str());

  delay(2000);
}
