/*
 * File: camera_handler.h
 * Description: Provides functions for initializing and capturing images using the ArduCam Mini 2MP.
 */

#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include <ArduCAM.h>
#include <SPI.h>
#include "pins.h"

// Camera instance
extern ArduCAM myCAM;

// Function declarations
void initCamera();
bool captureImage(uint8_t *buffer, size_t &length);

#endif // CAMERA_HANDLER_H
