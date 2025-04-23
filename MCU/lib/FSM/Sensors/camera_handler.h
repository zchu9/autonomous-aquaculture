#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include <Arduino.h>
#include <ArduCAM.h>

// A simple handler class for the ArduCAM Mini 2MP Plus.
class CameraHandler
{
public:
    CameraHandler();

    // Initialize SPI, I2C, and the camera module.
    void begin();

    // Capture an image and store it in the ArduCAM FIFO.
    // Returns the image size in bytes or 0 on error.
    uint32_t captureImage();

    // Prepare for streaming the captured image in chunks.
    void startImageStream();

    // Read the next chunk of image data from FIFO into buffer.
    // chunkSize: maximum number of bytes to read.
    // Returns the actual number of bytes read.
    uint16_t readImageChunk(uint16_t chunkSize, uint8_t *buffer);

    // Finish the image stream (closes the burst read).
    void finishImageStream();

private:
    bool validateModel();

    uint32_t imgLength;  // Total captured image length.
    uint32_t currentPos; // Current read position in the FIFO.
};

#endif