#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H
#pragma once

#include <Arduino.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <pins.h>

#define CAM_TYPE OV2640

// External camera instance
extern ArduCAM myCAM;

void initCamera();
uint8_t *captureImage();
bool sendCapturedImage();
size_t getCapturedImageSize();
void getPartialImage(char* buffer, size_t bufferSize);

#endif