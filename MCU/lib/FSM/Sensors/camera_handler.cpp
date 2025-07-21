#include <SPI.h>
#include <Wire.h>
#include "pins.h"
#include "camera_handler.h"
#define DEBUG 1
// Create an instance of the ArduCAM class.
// We use OV2640 (a valid numeric constant defined in ArduCAM.h) as the sensor model.
//! The memorysaver.h file configures the module as OV2640 Mini 2MP Plus.
ArduCAM myCAM{OV2640, CS_PIN};

static int imageLength = 0;
static int currentPos = 0;
// Resolution selection – choose your desired resolution.
// Uncomment the one you wish to use.
// #define JPEG_RESOLUTION OV2640_160x120     // 160 x 120 resolution
// #define JPEG_RESOLUTION OV2640_176x144     // 176 x 144 resolution
// #define JPEG_RESOLUTION OV2640_320x240     // 320 x 240 resolution (default)
#define JPEG_RESOLUTION OV2640_640x480 // 640 x 480 resolution (VGA)
// #define JPEG_RESOLUTION OV2640_800x600     // 800 x 600 resolution (SVGA)
// #define JPEG_RESOLUTION OV2640_1024x768    // 1024 x 768 resolution (XGA)
// #define JPEG_RESOLUTION OV2640_1280x1024   // 1280 x 1024 resolution (SXGA)
// #define JPEG_RESOLUTION OV2640_1600x1200   // 1600 x 1200 resolution (UXGA)
#include <ArduinoTrace.h>
void CameraHandler::begin()
{
    TRACE();
    // Initialize SPI.
    SPI.begin();

    // Set the chip select pin.
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);

    // Reset the CPLD
    myCAM.write_reg(0x07, 0x80);
    delay(100);
    myCAM.write_reg(0x07, 0x00);
    delay(100);

    // Initialize I2C.
    Wire.begin();

    uint8_t temp;
    // Test communication with the camera module.
    // Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
// Serial.println(F("ACK CMD SPI interface Error! END"));
        delay(1000);
    }
    else
    {
// Serial.println(F("ACK CMD SPI interface OK. END"));
    }

    // Set the image format to JPEG.
    myCAM.set_format(JPEG);

    // Initialize the camera. Uses I2C.
    myCAM.InitCAM();
    // Configure the sensor resolution. Also uses I2C.
    myCAM.OV2640_set_JPEG_size(JPEG_RESOLUTION);

    myCAM.CS_LOW();
    myCAM.clear_fifo_flag(); // Uses SPI!
    myCAM.CS_HIGH();
#ifdef DEBUG
    validateModel();
#endif
}

// depreciated in favor of segmented image transfers.
uint32_t CameraHandler::captureImage()
{
    // Reset image length and position.
    imageLength = 0;
    currentPos = 0;

    myCAM.CS_LOW();

    // Flush any residual data and clear FIFO flags.
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();

    // Start image capture.
    myCAM.start_capture();
// Serial.println("Capturing image...");

    // Wait until capture is complete (with a timeout).
    const unsigned long timeout = 5000; // 5000 ms timeout.
    unsigned long startTime = millis();

    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    {
        if (millis() - startTime > timeout)
        {
// Serial.println("Error: Capture timed out.");
            return 0;
        }
    }
// Serial.print(imageLength);
    // Retrieve the length (in bytes) of the captured image.
    // This function returns (value & 0x07fffff).
    imageLength = myCAM.read_fifo_length();

    if ((imageLength >= 0x7FFFFF) || (imageLength == 0))
    {
// Serial.println("Error: Image length is invalid.");
        return 0;
    }

// Serial.print("Image captured. Size: ");
// Serial.print(imageLength);
// Serial.println(" bytes.");

    return imageLength;
}

void CameraHandler::startImageStream()
{
    // Reset the current position.
    currentPos = 0;
    // Begin burst read from FIFO.
    myCAM.CS_LOW();
    myCAM.set_fifo_burst(); // equivalent to SPI.transfer(0x3C) or SPI.transfer(BURST_FIFO_READ);

// Serial.println("start reading pal");
}

uint16_t CameraHandler::readImageChunk(uint16_t chunkSize, uint8_t *buffer)
{
    uint16_t bytesRead = 0;
    // Read until either the requested chunk size is reached or no more bytes.
    for (uint16_t i = 0; i < chunkSize && currentPos < imageLength; i++, currentPos++)
    {
        buffer[i] = SPI.transfer(0x00);
        bytesRead++;
    }
    return bytesRead;
}

void CameraHandler::finishImageStream()
{
    // empty buffer, then drive CSn.
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();

    myCAM.CS_HIGH();

// Serial.println("cutting off stream (ouch)");
}

bool CameraHandler::validateModel()
{
    uint8_t vid, pid;
    // Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26) && ((pid != 0x41) || (pid != 0x42)))
    {
// Serial.println(F("ACK CMD Can't find OV2640 module! END"));
        return false;
    }
    else
    {
// Serial.println(F("ACK CMD OV2640 detected. END"));
        return true;
    }
}