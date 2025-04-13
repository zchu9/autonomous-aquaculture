// each winch has a lift/lower pin
// each winch has a height sensor

// TODO:
// add an initialize function for class variables and winchID.

#include <Arduino.h>
#include "Sensors/pot_handler.h" // for getHeight
#include "pins.h"

class winchData
{
    // pins
    pin_size_t winchLiftPin;
    pin_size_t winchLowerPin;
    pin_size_t heightSensorPin;

    // values
    int winchID; // 0-3
    double currentHeight;

public:
    winchData(pin_size_t winchLiftPin = -1, pin_size_t winchLowerPin = -1, pin_size_t heightSensorPin = -1);
    bool lower(double height);
    bool lift(double height);
    bool selectWinchLine(bool lift);
};
