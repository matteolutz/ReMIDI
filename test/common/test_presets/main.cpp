#include <unity.h>

#if !(ARDUINO)
#include "../eepromsim.h"
#endif

#include <ReMIDIPresets.h>

/** This test has to run first (it expects the preset list to be empty) */
void test_ensurePresetListHeader()
{
    remidi::ReMIDIPresetListHeader header = remidi::ensurePresetList();

    TEST_ASSERT_EQUAL(REMIDI_PRESET_LIST_HEADER_MAGIC, header.magic);
    TEST_ASSERT_EQUAL(-1, header.firstPresetAddress);
}

/** This test has to run second or before any preset with PC `42` is stored */
void test_findInexistentPreset()
{
    remidi::ReMIDILoadedPreset preset = remidi::findPresetForPCNumber(42);
    TEST_ASSERT(!remidi::isPresetValid(preset.preset));
}

void test_storeAndFindPresetSingle()
{
    remidi::ReMIDIPreset preset;
    preset.pcNumber = 69;
    preset.nextPresetAddress = -1;
    preset.controlCount = 2;

    remidi::ReMIDIControlState controlStates[2];
    controlStates[0].controlId = 1;
    controlStates[0].state = 1;

    controlStates[1].controlId = 2;
    controlStates[1].state = 0;

    bool storeSuccess = remidi::storePreset(preset, controlStates);
    TEST_ASSERT(storeSuccess);

    remidi::ReMIDILoadedPreset loadedPreset = remidi::findPresetForPCNumber(69);
    TEST_ASSERT(remidi::isPresetValid(loadedPreset.preset));

    TEST_ASSERT_EQUAL(preset.pcNumber, loadedPreset.preset.pcNumber);
    TEST_ASSERT_EQUAL(preset.controlCount, loadedPreset.preset.controlCount);
    TEST_ASSERT_EQUAL(-1, loadedPreset.preset.nextPresetAddress);

    remidi::ReMIDIControlState loadedControlStates[2];
    remidi::loadPresetControlStates(loadedPreset, loadedControlStates);

    TEST_ASSERT_EQUAL(controlStates[0].controlId, loadedControlStates[0].controlId);
    TEST_ASSERT_EQUAL(controlStates[0].state, loadedControlStates[0].state);
    TEST_ASSERT_EQUAL(controlStates[1].controlId, loadedControlStates[1].controlId);
    TEST_ASSERT_EQUAL(controlStates[1].state, loadedControlStates[1].state);
}

/** Test storing a preset with the same PC number as an existing one (we are depending on the `test_storeAndFindPresetSingle`) */
void test_storePresetWithSamePCNumber()
{
    remidi::ReMIDIPreset preset;
    preset.pcNumber = 69;
    preset.nextPresetAddress = -1;
    preset.controlCount = 2; // same control count as the existing preset

    remidi::ReMIDIControlState controlStates[2];
    controlStates[0].controlId = 1;
    controlStates[0].state = 12;

    controlStates[1].controlId = 2;
    controlStates[1].state = 13;

    bool storeSuccess = remidi::storePreset(preset, controlStates);
    TEST_ASSERT(storeSuccess);

    remidi::ReMIDILoadedPreset loadedPreset = remidi::findPresetForPCNumber(69);
    TEST_ASSERT(remidi::isPresetValid(loadedPreset.preset));

    TEST_ASSERT_EQUAL(preset.pcNumber, loadedPreset.preset.pcNumber);
    TEST_ASSERT_EQUAL(preset.controlCount, loadedPreset.preset.controlCount);
    TEST_ASSERT_EQUAL(-1, loadedPreset.preset.nextPresetAddress);

    remidi::ReMIDIControlState loadedControlStates[2];
    remidi::loadPresetControlStates(loadedPreset, loadedControlStates);

    TEST_ASSERT_EQUAL(controlStates[0].controlId, loadedControlStates[0].controlId);
    TEST_ASSERT_EQUAL(controlStates[0].state, loadedControlStates[0].state);
    TEST_ASSERT_EQUAL(controlStates[1].controlId, loadedControlStates[1].controlId);
    TEST_ASSERT_EQUAL(controlStates[1].state, loadedControlStates[1].state);

    // hacky way to make sure, that the preset was actually overwritten, and not just added to the end of the list
    remidi::ReMIDIPresetListHeader header = remidi::ensurePresetList();
    TEST_ASSERT_EQUAL(header.firstPresetAddress, loadedPreset.controlStatesAddress - sizeof(remidi::ReMIDIPreset));
}

/** Store another preset after PC 42 (depnds on `test_storeAndFindPresetSingle`) */
void test_storeAndFindPresetsMultiple()
{
    remidi::ReMIDIPreset newPreset;
    newPreset.pcNumber = 42;
    newPreset.nextPresetAddress = -1;
    newPreset.controlCount = 2; // same control count as the existing preset

    remidi::ReMIDIControlState newControlStates[2];
    newControlStates[0].controlId = 1;
    newControlStates[0].state = 40;

    newControlStates[1].controlId = 2;
    newControlStates[1].state = 41;

    bool storeSuccess = remidi::storePreset(newPreset, newControlStates);
    TEST_ASSERT(storeSuccess);

    remidi::ReMIDILoadedPreset loadedPreset = remidi::findPresetForPCNumber(69);
    TEST_ASSERT(remidi::isPresetValid(loadedPreset.preset));
    TEST_ASSERT_EQUAL(69, loadedPreset.preset.pcNumber);
    TEST_ASSERT_EQUAL(2, loadedPreset.preset.controlCount);
    TEST_ASSERT_NOT_EQUAL(-1, loadedPreset.preset.nextPresetAddress); // make sure, the new preset (42) was storedA

    remidi::ReMIDILoadedPreset newLoadedPreset = remidi::findPresetForPCNumber(42);
    TEST_ASSERT(remidi::isPresetValid(newLoadedPreset.preset));
    TEST_ASSERT_EQUAL(42, newLoadedPreset.preset.pcNumber);
    TEST_ASSERT_EQUAL(2, newLoadedPreset.preset.controlCount);
    TEST_ASSERT_EQUAL(-1, newLoadedPreset.preset.nextPresetAddress);

    remidi::ReMIDIControlState newLoadedControlStates[2];
    remidi::loadPresetControlStates(newLoadedPreset, newLoadedControlStates);
    TEST_ASSERT_EQUAL(newControlStates[0].controlId, newLoadedControlStates[0].controlId);
    TEST_ASSERT_EQUAL(newControlStates[0].state, newLoadedControlStates[0].state);
    TEST_ASSERT_EQUAL(newControlStates[1].controlId, newLoadedControlStates[1].controlId);
    TEST_ASSERT_EQUAL(newControlStates[1].state, newLoadedControlStates[1].state);
}

int runUnityTests()
{
    UNITY_BEGIN();

    RUN_TEST(test_ensurePresetListHeader);
    RUN_TEST(test_findInexistentPreset);
    RUN_TEST(test_storeAndFindPresetSingle);
    RUN_TEST(test_storePresetWithSamePCNumber);
    RUN_TEST(test_storeAndFindPresetsMultiple);

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