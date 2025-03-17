#include <OneWire.h>
#include <DallasTemperature.h>
#include "pins.h"
#include "mqtt_handler.ino"
#include "camera_handler.h"
#include "config.h"

#define BUFFER_SIZE 65536  // Adjust based on memory constraints
uint8_t imgBuffer[BUFFER_SIZE];
size_t imgSize;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
    // Initialize MQTT
    setupMQTT(); // Connects to WiFi and MQTT
    
    // Initialize Temperature Sensor
    sensors.begin();
    
    // Initialize Camera
    initCamera();
    Serial.println("Camera initialized.");
}

void loop() {
    // Read Potentiometer
    int potValue = analogRead(POT_PIN);
    
    // Read Temperature Sensor
    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);
    float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;
    
    // Capture Image
    Serial.println("Capturing image...");
    if (captureImage(imgBuffer, imgSize)) {
        Serial.println("Image captured successfully.");
        // Further processing or transmission of imgBuffer can be done here
    } else {
        Serial.println("Image capture failed.");
    }
    
    // Publish Sensor Data
    publishSensorData(potValue, temperatureC, temperatureF);
    
    delay(5000);  // Send data every 5 seconds
}

