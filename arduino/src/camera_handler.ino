/*
 * File: camera_handler.ino
 * Description: Implements camera initialization and image capture using the ArduCam Mini 2MP.
 */

#include "camera_handler.h"

// Define the ArduCAM object
ArduCAM myCAM(OV2640, CAMERA_CS_PIN);

// Function to initialize the camera
void initCamera() {
    pinMode(CAMERA_CS_PIN, OUTPUT);
    SPI.begin();
    
    myCAM.write_reg(0x07, 0x80);
    delay(100);
    myCAM.write_reg(0x07, 0x00);
    delay(100);
    
    myCAM.InitCAM();
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
    myCAM.clear_fifo_flag();
    myCAM.OV2640_set_JPEG_size(OV2640_1600x1200); // Adjust resolution if needed
    delay(1000);
}

// Function to capture an image
bool captureImage(uint8_t *buffer, size_t &length) {
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
    
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
    
    length = myCAM.read_fifo_length();
    if (length >= 65536 || length == 0) {
        return false;
    }
    
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();
    
    for (size_t i = 0; i < length; i++) {
        buffer[i] = SPI.transfer(0x00);
    }
    
    myCAM.CS_HIGH();
    return true;
}
