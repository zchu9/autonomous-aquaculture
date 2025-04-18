#include "camera_handler.h"

enum SystemState
{
  STATE_IDLE,
  STATE_CAPTURING,
  STATE_STREAMING,
  STATE_ERROR
};

SystemState currentState = STATE_IDLE;
CameraHandler camera;

const uint16_t CHUNK_SIZE = 200; // Adjust based on LoRa packet limits.
uint8_t buffer[CHUNK_SIZE];
uint32_t imageSize = 0;
uint16_t bytesRead = 0;

void setup()
{
  Serial.begin(9600);
  delay(2000); // Allow time for Serial and hardware initialization.

  camera.begin();
  Serial.println("System ready. Type 'CAPTURE' and press enter to capture an image.");
}

void loop()
{
  switch (currentState)
  {
  case STATE_IDLE:
    // Wait for user command via Serial.
    if (Serial.available() > 0)
    {
      String command = Serial.readStringUntil('\n');
      command.trim();
      if (command.equals("CAPTURE"))
      {
        currentState = STATE_CAPTURING;
      }
    }
    break;

  case STATE_CAPTURING:
    imageSize = camera.captureImage();
    if (imageSize > 0)
    {
      Serial.print("Image captured. Size: ");
      Serial.print(imageSize);
      Serial.println(" bytes.");
      // Prepare for streaming.
      camera.startImageStream();
      currentState = STATE_STREAMING;
    }
    else
    {
      Serial.println("Error during capture.");
      currentState = STATE_ERROR;
    }
    break;

  case STATE_STREAMING:
    // Read image in chunks.
    bytesRead = camera.readImageChunk(CHUNK_SIZE, buffer);
    if (bytesRead > 0)
    {
      // Here, we're writing to Serial for testing.
      // In your LoRa implementation, replace Serial.write() with your LoRa send function.
      Serial.write(buffer, bytesRead);
      delay(10); // adjust delay to suit communication.
    }
    // When no more data is left, finish the stream.
    if (bytesRead == 0)
    {
      camera.finishImageStream();
      Serial.println();
      Serial.println("Finished streaming image.");
      currentState = STATE_IDLE;
    }
    break;

  case STATE_ERROR:
    // Handle errors
    Serial.println("An error occurred. Resetting to idle state.");
    currentState = STATE_IDLE;
    break;

  default:
    currentState = STATE_IDLE;
    break;
  }
}
