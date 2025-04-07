/*
 * File: temperature_sensor.cpp
 * Description: DS18B20 temperature sensor interface returning Celsius and Fahrenheit.
 */

 #include "temperature_sensor.h"
 #include "pins.h"
 #include <OneWire.h>
 #include <DallasTemperature.h>
 
 OneWire oneWire(ONE_WIRE_BUS);
 DallasTemperature sensors(&oneWire);
 
 void initTemperatureSensor() {
     sensors.begin();
 }
 
 float getTempC() {
     sensors.requestTemperatures();
     return sensors.getTempCByIndex(0);
 }
 
 float getTempF() {
     float c = getTempC();
     return c * 9.0 / 5.0 + 32.0;
 }
 