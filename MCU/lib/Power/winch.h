// each winch has a lift/lower pin
// each winch has a height sensor

// TODO:
// add an initialize function for class variables and winchID.

#include <Arduino.h>
#include "FSM.h"    // for getHeight

class winchData {
    // pins
    pin_size_t winchLiftPin;
    pin_size_t winchLowerPin;
    pin_size_t heightSensorPin;

    // values
    int winchID;    // 0-3
    double currentHeight;

public:
    winchData(pin_size_t winchLiftPin = -1, pin_size_t winchLowerPin = -1, pin_size_t heightSensorPin = -1);
    bool lower(double height);
    bool lift(double height);
    bool selectWinchLine(bool lift);
};



/**
 * @brief This function will select the proper multiplexer output based on the winchID.
 * There can be 4 winches on the current implementation. The low pins are associated with lifts and the high pins are associated with lowering.
 * To get from the low pin to the high pin, add 4 to the winch ID.
 * Example: winchID 0 will lift and lower on mux outputs 0 and 4; winchID 1 will lift and lower on mux outputs 1 and 5, respectively.
 *
 * @param lift
 * @return true
 * @return false
 */
bool winchData::selectWinchLine(bool lift) {
    int index = this->winchID;
    if (!lift) {
        index += 4;
    }

    digitalWrite(MUX_DISABLE_2, HIGH);
    digitalWrite(MUX_SEL_0, (index & 0x001));
    digitalWrite(MUX_SEL_1, (index & 0x010));
    digitalWrite(MUX_SEL_2, (index & 0x100));
    digitalWrite(MUX_DISABLE_2, LOW);
    return true;
};

bool winchData::lower(double desiredHeight) {
    int numIterations = 0;
    int beginningHeight = getHeight();

    selectWinchLine(false);

    pin_size_t lowerPin = this->winchLowerPin;
    digitalWrite(lowerPin, HIGH);

    while (getHeight() > desiredHeight)
    {
        numIterations++;
        if (numIterations % 25 == 0)
        {
            if (beginningHeight - getHeight() < 0.1)
            {
                // we are stuck
                digitalWrite(lowerPin, LOW);
                return false;
            }
            beginningHeight = getHeight();
        }
    }
    digitalWrite(lowerPin, LOW);
    this->currentHeight = getHeight();
    return true;
};

bool winchData::lift(double desiredHeight) {
    int numIterations = 0;
    int beginningHeight = getHeight();

    selectWinchLine(true);

    pin_size_t liftPin = this->winchLiftPin;
    digitalWrite(liftPin, HIGH);

    while (getHeight() > desiredHeight)
    {
        numIterations++;
        if (numIterations % 25 == 0)
        {
            if (beginningHeight - getHeight() < 0.1)
            {
                // we are stuck
                digitalWrite(liftPin, LOW);
                return false;
            }
            beginningHeight = getHeight();
        }
    }
    digitalWrite(liftPin, LOW);
    this->currentHeight = getHeight();
    return true;
};