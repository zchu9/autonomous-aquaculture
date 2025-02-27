#include "pins.h"
// --- Pin Assignments ---
#define POT_PIN A0       // Potentiometer input

void setup() {
  Serial.begin(9600);  // Start Serial Monitor
  pinMode(POT_PIN, INPUT);  
}

void loop() {
  // --- Read Raw Potentiometer Value ---
  int potValue = analogRead(POT_PIN);  // Get raw value (0-4095)  - SAMD21 ADC is 12 bits.


  // --- Display Readings in Serial Monitor ---
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);

  delay(500);  // Update twice per second
}
