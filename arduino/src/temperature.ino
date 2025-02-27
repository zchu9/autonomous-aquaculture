#include <OneWire.h>            // blocking, disables interrupts for crit sections
#include <DallasTemperature.h>  // blocking
#include "pins.h"
// Pin 2
// Setup a OneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass OneWire reference to DallasTemperature sensor
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  Serial.println("DS18B20 Temperature Sensor");

  sensors.begin(); // Start the DS18B20 sensor
}

void loop() {
  sensors.requestTemperatures(); // Send command to get temperatures
  float temperatureC = sensors.getTempCByIndex(0); // Get temperature in Celsius
  float temperatureF = sensors.getTempFByIndex(0);

  if (temperatureC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.print(" °C  |  ");
    Serial.print(temperatureF);
    Serial.println(" °F");
  }

  delay(1000); // Wait 1 second before next reading
}
