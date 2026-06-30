#ifndef REMIDI_PRESETS_H
#define REMIDI_PRESETS_H

#include "ReMIDITypes.h"
#include "ReMIDIControl.h"

#define REMIDI__PRESET_LIST_HEADER_MAGIC 0x524D4449 // "RMDI" in ASCII

namespace remidi
{

    /**
     * The header of the preset list that is stored in EEPROM.
     * This will always be stored at EEPROM address 0
     * */
    struct __attribute__((packed)) ReMIDIPresetListHeader
    {
        uint32_t magic = REMIDI__PRESET_LIST_HEADER_MAGIC; // Magic number to validate the header

        int firstPresetAddress = -1;
    };

    /**
     * This will overide the current preset list header in EEPROM, be careful!
     */
    ReMIDIPresetListHeader initializePresetList();

    /**
     * This will check if the preset list header in EEPROM is valid, and if not, it will initialize it.
     */
    ReMIDIPresetListHeader ensurePresetList();

    uint16_t getPresetCount();

    struct __attribute__((packed)) ReMIDIPreset
    {
        /** The address of the next preset in EEPROM */
        int nextPresetAddress = -1;

        /**
         * The program change number to listen for
         * If set to -1, this preset is invalid
         */
        int8_t pcNumber = -1;

        /** The number of controls for this preset */
        uint8_t controlCount;
    };

    struct ReMIDILoadedPreset
    {
        /** The actual preset */
        ReMIDIPreset preset;

        /** The EEPROM address where the control states for this preset are stored */
        int controlStatesAddress;
    };

    bool isPresetValid(const ReMIDIPreset &preset);

    ReMIDILoadedPreset findPresetForPCNumber(uint8_t pcNumber);

    /**
     * This function will load the control states for the given preset into the provided buffer.
     * The controlStatesBuffer should have enough space to hold preset.controlCount control states.
     */
    void loadPresetControlStates(const ReMIDILoadedPreset &preset, ReMIDIControlState *controlStatesBuffer);

    /**
     * This function will return the EEPROM address where the given preset should be stored.
     * It will search for a preset with the same PC number, and if it finds one, it will return the address of that preset (to overwrite it).
     * If it doesn't find one, it will return the address where a new preset can be stored (which is right after the last preset in the list).
     */
    int getNextPresetAddressFor(ReMIDIPreset &preset);

    /**
     * This function will store the given preset in EEPROM.
     * If there is already a preset with the same PC number and control count, it will overwrite that preset, otherwise it will add a new preset to the end of the list.
     *
     * The controlStates array should have the same number of elements as preset.controlCount, and should contain the control states for the preset.
     */
    bool storePreset(ReMIDIPreset &preset, const ReMIDIControlState *controlStates);

}

#endif // REMIDI_PRESETS_H