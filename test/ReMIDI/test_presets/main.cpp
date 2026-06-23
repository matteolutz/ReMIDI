#include <unity.h>

#if !(ARDUINO)
#include <ReMIDIEEPROMSim.h>

#define EEPROM_SIZE 1024

uint8_t eepromData[EEPROM_SIZE];
remidi::ReMIDIEEPROMSimClass EEPROM(eepromData, EEPROM_SIZE);
#endif

#include <ReMIDIPresets.h>

void test_ensurePresetListHeader()
{
    remidi::ReMIDIPresetListHeader header = remidi::ensurePresetList();

    TEST_ASSERT_EQUAL(REMIDI_PRESET_LIST_HEADER_MAGIC, header.magic);
    TEST_ASSERT_EQUAL(-1, header.firstPresetAddress);
}

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

void test_storeAndFindPresetsMultiple() {}

int runUnityTests()
{
    UNITY_BEGIN();

    RUN_TEST(test_ensurePresetListHeader);
    RUN_TEST(test_findInexistentPreset);
    RUN_TEST(test_storeAndFindPresetSingle);
    RUN_TEST(test_storePresetWithSamePCNumber);

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