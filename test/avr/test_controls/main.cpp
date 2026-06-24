#include <unity.h>

#if !(ARDUINO)
#include "../eepromsim.h"
// #include <ArduinoFake.h>
#endif

int runUnityTests()
{
    UNITY_BEGIN();
    return UNITY_END();
}

int main()
{
    return runUnityTests();
}

#if ARDUINO
void setup()
{
    delay(2000);
    runUnityTests();
}
void loop() {}
#endif