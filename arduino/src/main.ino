#include <OneWire.h>
#include <DallasTemperature.h>
#include "pins.h"
#include "mqtt_handler.ino"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  setupMQTT(); // Connects to WiFi and MQTT
  sensors.begin();
}

void loop() {
  int potValue = analogRead(POT_PIN);
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;

  publishSensorData(potValue, temperatureC, temperatureF);
  delay(5000); // Send data every 5 seconds
}
