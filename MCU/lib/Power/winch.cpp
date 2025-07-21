
#include "winch.h"
winchData::winchData(pin_size_t winchLiftPin, pin_size_t winchLowerPin, pin_size_t heightSensorPin)
{
    this->winchLiftPin = winchLiftPin;
    this->winchLowerPin = winchLowerPin;
    this->heightSensorPin = heightSensorPin;
    this->currentHeight = 0;
    this->winchID = 0;
    analogReadResolution(12);
    pinMode(this->winchLiftPin, OUTPUT);
    pinMode(this->winchLowerPin, OUTPUT);
    pinMode(this->heightSensorPin, INPUT);
    digitalWrite(this->winchLiftPin, LOW);
    digitalWrite(this->winchLowerPin, LOW);
}

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
bool winchData::selectWinchLine(bool lift)
{
    int index = this->winchID;
    if (!lift)
    {
        index += 4;
    }

    digitalWrite(MUX_DISABLE_2, HIGH);
    digitalWrite(MUX_SEL_0, (index & 0x001));
    digitalWrite(MUX_SEL_1, (index & 0x010));
    digitalWrite(MUX_SEL_2, (index & 0x100));
    digitalWrite(MUX_DISABLE_2, LOW);
    return true;
};

bool winchData::lower(double desiredHeight)
{
    // int numIterations = 1;
    // int beginningHeight = getHeight();

    selectWinchLine(false);

    pin_size_t lowerPin = this->winchLowerPin;

    while (getHeight() > desiredHeight)
    {
        digitalWrite(lowerPin, HIGH);
// Serial.println(getHeight());
    }
    digitalWrite(lowerPin, LOW);
    this->currentHeight = getHeight();
    return true;
};

bool winchData::lift(double desiredHeight)
{
    // int numIterations = 1;
    // int beginningHeight = getHeight();

    selectWinchLine(true);

    pin_size_t liftPin = this->winchLiftPin;

    while (getHeight() < desiredHeight)
    {
        digitalWrite(liftPin, HIGH);
// Serial.println(getHeight());
    }
    digitalWrite(liftPin, LOW);
    this->currentHeight = getHeight();
    return true;
};
