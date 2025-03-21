#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H
#pragma once

#include <Arduino.h>
#include <ArduCAM.h>
#include <SPI.h>

#define CS_PIN 7
#define CAM_TYPE OV2640

// External camera instance
extern ArduCAM myCAM;

void initCamera();
bool captureImage();
bool sendCapturedImage();

#endif
