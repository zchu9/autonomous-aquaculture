#include <Arduino.h>
#include "camera_handler.h"

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // Wait for Serial to be ready (only needed for native USB)
    Serial.println("🔌 Booting...");
    initCamera();
}

void loop()
{
    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command.equalsIgnoreCase("Capture"))
        {
            Serial.println("[Arduino] Capture command received.");

            if (captureImage())
            {
                Serial.println("Image captured successfully.");
                sendImageOverSerial(); // Send the image to your computer over Serial
            }
            else
            {
                Serial.println("Failed to capture image.");
            }
        }
    }
}
