// 3.04.2025
// Arduino Uno R3 driver for generating data and sending it over UART for MKR1500.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define MODE_PIN 11
#define SEND_PIN 12

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool mode = true;

void setup() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;  // Don't proceed, loop forever, absolutely borked
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    pinMode(MODE_PIN, INPUT_PULLUP);
    pinMode(SEND_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, mode);

    Serial.begin(19200, SERIAL_8N1);
}

unsigned long time = 0;
bool flag = false;

void loop() {
    modeButton();
    dataButton();
}

void modeButton() {
    // swaps the message type from Victron to Renogy stucture.
    static unsigned long debounceTime = 0;
    unsigned long wait = 20;

    static int ledState = HIGH;
    static int buttonState;
    static int lastButtonState = LOW;

    int reading = digitalRead(MODE_PIN);

    if (reading != lastButtonState) {
        // reset the debouncing timer
        debounceTime = millis();
    }

    if ((millis() - debounceTime) > wait) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                ledState = !ledState;
                mode = !mode;
            }
        }
    }
    digitalWrite(LED_BUILTIN, ledState);
    lastButtonState = reading;
}

void dataButton() {
    // sends a random generated serial message.
    static unsigned long debounceTime = 0;
    unsigned long wait = 20;

    static int ledState = HIGH;
    static int buttonState;
    static int lastButtonState = LOW;

    int reading = digitalRead(SEND_PIN);

    if (reading != lastButtonState) {
        debounceTime = millis();
    }

    if ((millis() - debounceTime) > wait) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                writeData(display, mode);
            }
        }
    }
    lastButtonState = reading;
}

void writeData(Adafruit_SSD1306& display, bool mode) {
    // write data to serial out.
    // mode will determine baud and message structure.

    randomSeed(analogRead(0));
    byte num;
    if (mode == false) {
        num = random(1100, 1400) / 10.0; // victron format.
    }
    else {
        num = random(1000, 4000) / 10.0;  // renogy mAh format.
    }
    char val[10];
    dtostrf(num, 5, 2, val);  // sprintf doesn't support %f on lib-avr.
    display.clearDisplay();
    display.setCursor(0, 0);
    display.write(val);
    display.display();

    char buffer[50];
    if (mode == false) {
        // victron
        snprintf(buffer, 50, "V\t%s\r\n", val);

        // switch.
        Serial.flush();
        Serial.begin(19200, SERIAL_8N1);
        while (Serial.available()) {
            Serial.read();
        }
        Serial.write(buffer);
    }
    else {
        // renogy
        byte high, low;
        high = ((num >> 8) & 0xFF);
        low = (num & 0xFF);

        // switch.
        Serial.flush();
        Serial.begin(9600, SERIAL_8E1);
        while (Serial.available()) {
            Serial.read();
        }
        for (int i = 0; i < 3; i++) {
            // since the bytes are discarded on MKR1500, just send anything.
            Serial.write(high);
            Serial.write(low);
        }
    }
}
