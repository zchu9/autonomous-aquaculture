/*
 * File: pot_handler.cpp
 * Description: Handles 10-turn potentiometer readings, conversion to feet,
 * and calibration including zero position and max/min height bounds.
 */
 #include <Arduino.h>
 #include "pot_handler.h"
 #include "pins.h"
 
 static int zeroPosition = 0;
 static int minRawValue = 0;
 static int maxRawValue = 1023;
 static float maxHeightFt = 6.0;  // default max height in feet
 
 void initPot() {
     pinMode(POT_PIN, INPUT);
 }
 
 int getRawPotValue() {
     return analogRead(POT_PIN);
 }
 
 float getHeight() {
     int currentValue = getRawPotValue();
     float height = ((float)(currentValue - zeroPosition) / (maxRawValue - minRawValue)) * maxHeightFt;
     return height;
 }
 
 void setZeroHeight() {
    zeroPosition = analogRead(POT_PIN);
    minRawValue = 0;
    maxRawValue = 1023;
 }

 
 void setMaxHeight(float feet) { //call setMaxHeightFeet(6.0);  // or whatever our range is from min to max
     maxHeightFt = feet;
 }
 
 void setMinHeight() {
    minRawValue = analogRead(POT_PIN);
 }
 