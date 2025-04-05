#include "FSM.h"
#include <unity.h>
void setUp(void)
{
    // This function is called before each test
    initializeDebug();
    data d;
    // populate power vector with random values
    for (size_t i = 0; i < d.numWinches; i++)
    {
        d.power.push_back(random(0, 100)); // random power values between 0 and 100
    }
    // populate height with a random value
    d.height = random(0, 100); // random height value between 0 and 100
    for (size_t i = 0; i < d.numWinches; i++)
    {
        d.temp = random(0, 100); // random lift flag values (0 or 1)
    }
}
void tearDown(void)
{
    // This function is called after each test
    // Clean up any resources if needed
    // For example, if you allocated memory, free it here
    if (d.img != nullptr)
    {
        free(d.img);
        d.img = nullptr;
    }
}
void test_FSM_initialization()
{
    initializeStartup(d);
    TEST_ASSERT_EQUAL(d.state, NORMAL);
    // Add more assertions to check other initializations if needed
}
void test_checkPower()
{
    // I don't know how this function is going to work.
}
void test_getHeight()
{
    // add davids height files and then check this shiiiiit.
}
void test_getImage()
{
    initCamera();
    d.img = captureImage();
    TEST_ASSERT_NOT_NULL(d.img);
}
void test_jsonify()
{
    JsonDocument dataJson = jsonify(d);
    // Check if the JSON document is not empty
    TEST_ASSERT_FALSE(dataJson.isNull());
    // serial print the JSON document to check if it is correctly formatted

    Serial.println("JSON Document:");
    serializeJson(d.doc, Serial);
    // varif
}
