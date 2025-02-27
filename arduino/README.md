Arduino Sensor & Display Project
This project includes two separate Arduino sketches:
1. Potentiometer Reader – Reads a potentiometer value and displays it via serial.
2. DS18B20 Temperature Sensor – Reads temperature in Celsius & Fahrenheit using the DS18B20 sensor.
3. Pins.h includes pin assignments and should be included as a header #include "pins.h"
Potentiometer Reader (`potentiometer.ino`)
Functionality:
- Reads the analog value from a potentiometer (0-1023).
- Displays the value in the Serial Monitor.

Pin Connections:
| Component          | Arduino Pin |
|--------------------|------------|
| Potentiometer (VCC) | 5V |
| Potentiometer (GND) | GND |
| Potentiometer (Signal) | A0 |
| Display Voltage Output | D9 (PWM) |



DS18B20 Temperature Sensor (`temperature_sensor.ino`)
Functionality:
- Reads temperature from a **DS18B20 waterproof sensor**.
- Converts to **Celsius and Fahrenheit**.
- Displays the values in the **Serial Monitor**.

Pin Connections:
| DS18B20 Pin | Arduino Pin |
|-------------|------------|
| VCC (Red)   | 5V |
| GND (Black) | GND |
| Data (Yellow) | D2 |
| **4.7kΩ Pull-up Resistor** | Between VCC & Data |


How to Use
1. Upload the desired sketch** (`potentiometer.ino` or `temperature_sensor.ino`) to your Arduino.
2. Open the Serial Monitor** (`9600 baud`).
3. Observe sensor readings in real-time.

---

Dependencies
Before running the **temperature sensor code**, install these libraries:
1. OneWire Library
   - Install via Arduino IDE:  
     `Sketch → Include Library → Manage Libraries → Search "OneWire" → Install`
2. DallasTemperature Library
   - Install via Arduino IDE:  
     `Sketch → Include Library → Manage Libraries → Search "DallasTemperature" → Install`

Still need to impliment data logging.


