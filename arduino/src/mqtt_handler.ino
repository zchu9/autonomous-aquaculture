#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "pins.h"
#include "config.h"

// --- MQTT Client Setup ---
WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

void connectToMQTT() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ArduinoClient")) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying...");
      delay(2000);
    }
  }
}

void publishSensorData(int potValue, float tempC, float tempF) {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["potentiometer"] = potValue;
  jsonDoc["temperature_C"] = tempC;
  jsonDoc["temperature_F"] = tempF;

  char jsonBuffer[256];
  serializeJson(jsonDoc, jsonBuffer);

  client.publish(MQTT_TOPIC, jsonBuffer);
  Serial.println("MQTT Message Sent: ");
  Serial.println(jsonBuffer);
}

void setupMQTT() {
  connectToWiFi();
  connectToMQTT();
}
