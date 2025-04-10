// camera_handler.h
#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <ArduCAM.h>
#include <memorysaver.h>

#if !(defined OV2640_MINI_2MP_PLUS)
#error Please select the hardware platform (OV2640_MINI_2MP_PLUS) in memorysaver.h
#endif

class CameraHandler {
public:
    CameraHandler(uint8_t csPin);

    void init();
    bool captureImage();
    bool isImageReady();
    uint8_t readFIFO();
    uint32_t getImageLength();
    void streamImage(HardwareSerial &serial);


private:
    ArduCAM* myCAM;
    uint8_t _csPin;
};

#endif // CAMERA_HANDLER_H
