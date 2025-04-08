#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H
#pragma once

/**
 *  Camera Handler Library
 *  ----------------------
 *  This library provides functions to capture and store images using an ArduCAM connected to an Arduino MKR NB 1500.
 *  The captured image is stored in memory as a JPEG and can be accessed via a pointer and size getter functions.
 *  Usage:
 *  ------
 *  1. Call `initCamera()` in your `setup()` function to initialize the camera hardware.
 *  2. Use `captureImage()` to capture an image and store it in memory.
 *  3. Access the stored image using `getImageBuffer()` and `getImageSize()` for transmission or further processing.
 *  Note:
 *  -----
 *  - This library does NOT save images to SD card or storage devices.
 *  - Image data remains in memory until overwritten by a new capture or explicitly freed.
 */

#include <Arduino.h>
// Prevent ArduCAM from redefining Serial
#ifdef ARDUINO_SAMD_MKRNB1500
#define Serial SerialUSB
#endif
#include <ArduCAM.h>
#include <SPI.h>
#include <pins.h>

#define CAM_TYPE OV2640 // Camera type

// External camera instance
extern ArduCAM myCAM;

void initCamera();
bool captureImage();
uint8_t *getImageBuffer();
size_t getImageSize();

void sendImageOverSerial();

#endif // CAMERA_HANDLER_H
