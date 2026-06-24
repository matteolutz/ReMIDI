#include <unity.h>

#if !(ARDUINO)
#include "../eepromsim.h"
#else
#include <Arduino.h>
#include <EEPROM.h>
#endif

template <typename T>
void test_writeAndRead(T value, int address)
{
    EEPROM.put(address, value);

    T readValue;
    EEPROM.get(address, readValue);

    TEST_ASSERT_EQUAL(value, readValue);
}

void test_writeAndReadByte()
{
    uint8_t testValue = 42;
    test_writeAndRead(testValue, 0);
}

void test_writeAndReadInt()
{
    int testValue = 42;
    test_writeAndRead(testValue, 0);
}

void test_writeAndReadStruct()
{
    struct TestStruct
    {
        uint8_t a;
        int b;
        float c;
    };

    TestStruct writeValue = {42, 12345, 3.14f};
    EEPROM.put(0, writeValue);

    TestStruct readValue;
    EEPROM.get(0, readValue);

    TEST_ASSERT_EQUAL(writeValue.a, readValue.a);
    TEST_ASSERT_EQUAL(writeValue.b, readValue.b);
    TEST_ASSERT_FLOAT_WITHIN(0.001, writeValue.c, readValue.c);
}

int runUnityTests()
{
    UNITY_BEGIN();

    RUN_TEST(test_writeAndReadByte);
    RUN_TEST(test_writeAndReadInt);
    RUN_TEST(test_writeAndReadStruct);

    return UNITY_END();
}

/* ---------------------------- Native entrypoint --------------------------- */
int main()
{
    return runUnityTests();
}

#if ARDUINO
/* --------------------------- Arduino entrypoint --------------------------- */
void setup()
{
    delay(2000); // wait for serial monitor to open
    runUnityTests();
}
void loop() {}
#endif