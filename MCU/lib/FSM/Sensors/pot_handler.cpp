/*
 * File: pot_handler.cpp
 * Description: Handles 10-turn potentiometer readings, conversion to feet,
 * and calibration including zero position and max/min height bounds.
 */
#include <Arduino.h>
#include "pot_handler.h"
#include "pins.h"

static int zeroPosition = 0;
static int minRawValue = 0;
static int maxRawValue = 4095;
static float maxHeightFt = 4.0; // default max height in feet

void initPot()
{
    pinMode(POT_PIN, INPUT);
}

int getRawPotValue()
{
    return analogRead(POT_PIN);
}

float getHeight()
{
    int currentValue = getRawPotValue();
    float height = ((float)(currentValue - zeroPosition) / (maxRawValue - minRawValue)) * maxHeightFt;
    return height;
}

void setZeroHeight()
{
    zeroPosition = analogRead(POT_PIN);

    // Give wiggle room and prevent mechanical limit damage
    int wiggle = 200;
    minRawValue = zeroPosition - ((zeroPosition - 0) > wiggle ? wiggle : 0);
    maxRawValue = zeroPosition + ((maxRawValue - zeroPosition) > wiggle ? wiggle : 0);
}

void setMaxHeight(float feet)
{
    maxHeightFt = feet;
}

void setMinHeight(float feet)
{
    // This function doesn't directly affect behavior since we're using maxHeightFt.
    // Reserved in case we switch to min/max ft-based clamping in future.
}
