/**
 * @file test_helpers.cpp
 * @author Daniel Reeves (dtr@vt.edu)
 * @brief function test cases for the system and stuff
 * @version 0.1
 * @date 2025-04-08
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "FSM.h"
#include <unity.h>
#include "LoRa.h"

#define NUM_DATA_POINTS 100
#define LORA_TESTS 0

// Mock functions for testing (replace with actual hardware functions)
int initialize_board(void)
{
    // Simulate board initialization (return 0 for success)
    return 0;
}

int check_board_response(void)
{
    // Simulate board response (return 1 for success)
    return 1;
}

// Test cases
void test_board_initialization(void)
{
    TEST_ASSERT_EQUAL_INT(0, initialize_board());
}

void test_board_response(void)
{
    TEST_ASSERT_EQUAL_INT(1, check_board_response());
}

data d;
LoraRadio radio;
void setUp(void)
{
    // This function is called before each test
    // initializeDebug();
    int powerStartPoint = 50;
    int sensorStartPoint = 0;
    // populate power vector with random values
    for (size_t i = 0; i < NUM_DATA_POINTS; i++)
    {

        d.power.push_back(powerStartPoint + i); // random power values between 0 and 100
    }
    // populate height with a random value

    for (size_t i = 0; i < NUM_DATA_POINTS; i++)
    {
        d.temp.push_back(random(0, 100)); // random lift flag values (0 or 1)
    }
    // d.img = image_data;
    initPot();
}
void tearDown(void)
{
    // This function is called after each test
    // Clean up any resources if needed
    // For example, if you allocated memory, free it here
}

void test_checkPower()
{
    // I don't know how this function is going to work.
}

void test_getHeight()
{
    Serial.println("This test will be interactive to verify the height at different real world positions:\n");
    Serial.println("Setting the max height:");
    setMaxHeight();
    Serial.println("Pot Value: " + String(getRawPotValue()));
    Serial.println("Height Value in Feet before setZeroHeight: " + String(getHeight()));
    Serial.println("-----------------------------");
    Serial.println("Move the cord to its limit and set the Min Height:");
    delay(10000);
    Serial.println("Pot Value: " + String(getRawPotValue()));
    Serial.println("Height Value in Feet before setZeroHeight: " + String(getHeight()));
    setZeroHeight();

    float currHeight = getHeight();
    TEST_ASSERT_TRUE(currHeight == 0);

    Serial.println("Height Value in feet after calibration: " + String(currHeight));
    Serial.println("Now move 1 foot up and Record the height again.");
    delay(10000);

    currHeight = getHeight();
    Serial.println("Height Value in Feet after moving up: " + String(currHeight));
    bool oneFootUp = (currHeight > 0.8 && currHeight < 1.2);
    TEST_ASSERT_TRUE(oneFootUp); // Allow a tolerance of 0.1 feet

    Serial.println("Height Value in Feet after moving up: " + String(currHeight));
}

void heightInstantiation()
{
    setMaxHeight();
    Serial.println("The max height is set.");
    Serial.println("pull ripcord out and wait:");
    delay(10000);
    setZeroHeight();
}

void pinInstantition()
{
    pinMode(LIFT_PIN, OUTPUT);
    digitalWrite(LIFT_PIN, LOW);
    pinMode(LOWER_PIN, OUTPUT);
    digitalWrite(LOWER_PIN, LOW);
}

void test_liftWinch()
{
    heightInstantiation();
    pinInstantition();
    // lift the winch from zero to 3.5 feet
}

void test_liftWinch_stuck()
{
    heightInstantiation();
}

void test_jsonify()
{
    JsonDocument dataJson = jsonify(d);

    // Check if the JSON document is not empty
    TEST_ASSERT_FALSE(dataJson.isNull());
    // serial print the JSON document to check if it is correctly formatted

    Serial.println("JSON Document:");
    Serial.println(serializeJson(d.doc, Serial));
}

// EVERYTHING BELOW WILL BE BLOCKED BY LORA MESSAGES

void test_sendData()
{
    LoraRadio radio;

    TEST_ASSERT_TRUE(radio.setupLoRa()); // Ensure LoRa is set up before sending data
    Serial.println("jsonify next");
    JsonDocument doc = jsonify(d);
    size_t len = measureJson(doc);
    char *buffer = new char[len + 1]; // +1 for null terminator
    Serial.print("Sending JSON: ");

    serializeJson(doc, buffer, len + 1);
    Serial.print("makes it past serializeJson: ");

    // Send the JSON over LoRa
    TEST_ASSERT_TRUE(sendData(d));
    delete[] buffer; // Free the allocated memory
}

void test_FSM_initialization()
{
    initializeStartup(d);
    TEST_ASSERT_EQUAL(d.state, NORMAL);
    // Add more assertions to check other initializations if needed
}

void runTests()
{
    UNITY_BEGIN();

    // Test tests
    RUN_TEST(test_board_initialization);
    RUN_TEST(test_board_response);

    // RUN_TEST(test_checkPower);
    // RUN_TEST(test_getImage);
    // RUN_TEST(test_jsonify);
    // RUN_TEST(test_getHeight);
    // RUN_TEST(test_liftWinch);

    // LoRa Dependent  Tests

    //RUN_TEST(test_sendData);

#if SYSTEM_TESTS
    test_FSM_initialization();
#endif
    UNITY_END();
}

void setup()
{
    // wait 2 seconds for the unity test runner
    // establishes a serial connection with the board
    delay(2000);
    runTests();
}

void loop()
{
}
