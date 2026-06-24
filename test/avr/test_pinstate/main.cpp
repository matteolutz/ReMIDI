#include <unity.h>
#include <Arduino.h>

#include "../pinstate.h"

void test_pinBasicMode(uint8_t pin, int mode)
{
    pinMode(pin, mode);

    auto pinState = getDigitalPinState(pin);

    TEST_ASSERT(pinState.isValid);
    TEST_ASSERT_EQUAL(pinState.mode, mode);
}

void test_inputPin()
{
    test_pinBasicMode(2, INPUT);
}

void test_inputPullupPin()
{
    test_pinBasicMode(3, INPUT_PULLUP);
}

void test_outputPin()
{
    test_pinBasicMode(4, OUTPUT);
}

void test_outputPinWithState()
{
    uint8_t pin = 5;
    test_pinBasicMode(pin, OUTPUT);

    digitalWrite(pin, HIGH);

    DigitalPinState pinHighState = getDigitalPinState(pin);
    TEST_ASSERT(pinHighState.isValid);
    TEST_ASSERT(pinHighState.isOutputAndHigh());

    digitalWrite(pin, LOW);

    DigitalPinState pinLowState = getDigitalPinState(pin);
    TEST_ASSERT(pinLowState.isValid);
    TEST_ASSERT(pinLowState.isOutputAndLow());
}

int runUnityTests()
{
    UNITY_BEGIN();

    RUN_TEST(test_inputPin);
    RUN_TEST(test_inputPullupPin);
    RUN_TEST(test_outputPin);

    RUN_TEST(test_outputPinWithState);

    return UNITY_END();
}

void setup()
{
    delay(2000);
    runUnityTests();
}
void loop() {}