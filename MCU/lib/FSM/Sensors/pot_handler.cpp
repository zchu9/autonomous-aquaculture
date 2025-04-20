/*
 * File: pot_handler.cpp
 * Description: Handles 10-turn potentiometer readings, conversion to feet,
 * and calibration including zero position and max/min height bounds.
 */
#include <Arduino.h>
#include "pot_handler.h"
#include "pins.h"

static int zeroPosition = 0;
static int maxHeight = 960;
static int minHeight = 200;
static float maxHeightFt = 4.0; // default max height in feet

void initPot()
{
    pinMode(POT_PIN, INPUT);
    analogReadResolution(12);
}

int getRawPotValue()
{
    return analogRead(POT_PIN);
}

// returns height in feet
float getHeight()
{
    float currentValue = getRawPotValue();
    float height = ((float)(currentValue - zeroPosition) / (maxHeight - minHeight)) * maxHeightFt;
    return height;
}
void setMaxHeight()
{
    maxHeight = analogRead(POT_PIN);
#if HARDCODE
    maxHeight = 960;
#endif
}

void setMinHeight()
{
    minHeight = analogRead(POT_PIN);
#if HARDCODE
    minHeight = 60;
#endif
}

void setZeroHeight()
{
    zeroPosition = analogRead(POT_PIN);
#if HARDCODE
#endif

    setMinHeight();
}

float getMaxHeightFeet()
{
    return maxHeightFt;
}