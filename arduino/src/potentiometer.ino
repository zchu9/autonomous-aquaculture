// --- Pin Assignments ---
#define POT_PIN A0       // Potentiometer input
#define DISPLAY_VOLTAGE_PIN 9  // Voltage output for 3-wire display (PWM)

void setup() {
  Serial.begin(9600);  // Start Serial Monitor
  pinMode(DISPLAY_VOLTAGE_PIN, OUTPUT);  // Set display pin as output
}

void loop() {
  // --- Read Raw Potentiometer Value ---
  int potValue = analogRead(POT_PIN);  // Get raw value (0-1023)


  // --- Display Readings in Serial Monitor ---
  Serial.print("Potentiometer Value: ");
  Serial.print(potValue);

  delay(500);  // Update twice per second
}
