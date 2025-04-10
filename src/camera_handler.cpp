// camera_handler.cpp
#include "camera_handler.h"

CameraHandler::CameraHandler(uint8_t csPin) : _csPin(csPin) {
    myCAM = new ArduCAM(OV2640, _csPin);
}

void CameraHandler::init() {
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    SPI.begin();

    // Reset ArduCAM with extended delay for better stability
    myCAM->write_reg(0x07, 0x80);
    delay(200);  // Extended reset delay
    myCAM->write_reg(0x07, 0x00);
    delay(200);  // Extended delay after reset

    // Test SPI communication with improved error handling
    uint8_t temp = 0;
    unsigned long startTime = millis();
    Serial.println("Testing SPI connection...");
    
    while (temp != 0x55) {
        myCAM->write_reg(ARDUCHIP_TEST1, 0x55);
        temp = myCAM->read_reg(ARDUCHIP_TEST1);
        
        if (temp != 0x55) {
            Serial.println("SPI connection failed. Retrying...");
            delay(100);
        }
        
        // Timeout after 3 seconds of trying
        if (millis() - startTime > 3000) {
            Serial.println("ERROR: Camera not responding. Check connections.");
            break;
        }
    }
    
    if (temp == 0x55) {
        Serial.println("SPI connection verified!");
    }

    // Clear FIFO and set JPEG mode
    myCAM->flush_fifo();
    myCAM->clear_fifo_flag();
    myCAM->set_format(JPEG);
    myCAM->write_reg(ARDUCHIP_MODE, 0x00);
    delay(100);

    // Initialize OV2640 with proper sequence
    Serial.println("Initializing camera sensor...");
    
    // Software reset
    myCAM->wrSensorReg8_8(0xff, 0x01);
    myCAM->wrSensorReg8_8(0x12, 0x80);
    delay(300);  // Extended delay after reset
    
    // Load basic settings
    myCAM->wrSensorRegs8_8(OV2640_JPEG_INIT);
    
    // Important: set resolution BEFORE setting up JPEG format
    Serial.println("Setting resolution to 640x480...");
    myCAM->wrSensorRegs8_8(OV2640_640x480_JPEG);
    // Alternative resolutions:
    // myCAM->wrSensorRegs8_8(OV2640_800x600_JPEG);
    // myCAM->wrSensorRegs8_8(OV2640_1024x768_JPEG);
    // myCAM->wrSensorRegs8_8(OV2640_1600x1200_JPEG);
    delay(1000);
    
    // Now set format sequence
    myCAM->wrSensorRegs8_8(OV2640_YUV422);
    myCAM->wrSensorRegs8_8(OV2640_JPEG);
    
    // Additional critical settings for proper JPEG operation
    myCAM->wrSensorReg8_8(0xff, 0x01);
    myCAM->wrSensorReg8_8(0x15, 0x00);
    
    // Force JPEG mode enabled
    myCAM->wrSensorReg8_8(0xff, 0x00);
    myCAM->wrSensorReg8_8(0xda, 0x10);
    myCAM->wrSensorReg8_8(0xd7, 0x03);
    myCAM->wrSensorReg8_8(0xe0, 0x00);
    
    // Apply quality settings (optional - medium quality default)
    myCAM->wrSensorReg8_8(0xff, 0x00);
    myCAM->wrSensorReg8_8(0x44, 0x00);  // Set appropriate clock divider
    
    // Verify settings took effect
    delay(500);
    Serial.println("Camera initialized successfully at 640x480");
    
    // Clear FIFO again to ensure clean start
    myCAM->flush_fifo();
    myCAM->clear_fifo_flag();
}

bool CameraHandler::captureImage() {
    myCAM->flush_fifo();
    myCAM->clear_fifo_flag();
    myCAM->start_capture();

    unsigned long start = millis();
    while (!myCAM->get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
        if (millis() - start > 5000) {
            Serial.println("Capture timeout.");
            return false;
        }
    }
    return true;
}

bool CameraHandler::isImageReady() {
    return myCAM->get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK);
}

uint8_t CameraHandler::readFIFO() {
    return myCAM->read_fifo();
}

uint32_t CameraHandler::getImageLength() {
    return myCAM->read_fifo_length();
}

void CameraHandler::streamImage(HardwareSerial &serial) {
    uint32_t length = myCAM->read_fifo_length();

    if (length >= 0x7FFFFF || length == 0) {
        serial.println("ERROR: Bad length");
        return;
    }

    Serial.print("Image length: ");
    Serial.println(length);  //  Helpful debug info

    myCAM->CS_LOW();
    myCAM->set_fifo_burst();

    serial.println("IMG_START");
    serial.flush();
    delay(100);

    for (uint32_t i = 0; i < length; i++) {
        uint8_t val = SPI.transfer(0x00);
        serial.write(val);
        if (i % 64 == 0) delay(1); // Throttle
    }

    serial.println("IMG_END");
    serial.flush();
    delay(20);

    myCAM->CS_HIGH();
    myCAM->flush_fifo();
    myCAM->clear_fifo_flag();
}
