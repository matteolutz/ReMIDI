#if ARDUINO
#include <EEPROM.h>
#else
#include <ReMIDIEEPROMSim.h>
#endif

#include "ReMIDIPresets.h"

namespace remidi
{
    ReMIDIPresetListHeader initializePresetList()
    {
        ReMIDIPresetListHeader header;
        EEPROM.put(0, header);
        return header;
    }

    ReMIDIPresetListHeader ensurePresetList()
    {
        ReMIDIPresetListHeader header;
        EEPROM.get(0, header);

        if (header.magic != REMIDI_PRESET_LIST_HEADER_MAGIC)
        {
            // this means, the preset list header is not valid
            return initializePresetList();
        }

        return header;
    }

    bool isPresetValid(const ReMIDIPreset &preset)
    {
        return preset.pcNumber != -1;
    }

    ReMIDILoadedPreset findPresetForPCNumber(uint8_t pcNumber)
    {
        ReMIDIPresetListHeader header = ensurePresetList();

        int presetAddress = header.firstPresetAddress;

        while (presetAddress != -1)
        {
            ReMIDIPreset preset;
            EEPROM.get(presetAddress, preset);

            if (preset.pcNumber == pcNumber)
            {
                ReMIDILoadedPreset loadedPreset;
                loadedPreset.preset = preset;
                loadedPreset.controlStatesAddress = presetAddress + sizeof(ReMIDIPreset);
                return loadedPreset;
            }

            presetAddress = preset.nextPresetAddress;
        }

        // If we reach this point, it means we didn't find a valid preset for the given PC number
        // We can return an invalid preset to indicate this
        ReMIDILoadedPreset invalidPreset;
        return invalidPreset;
    }

    void loadPresetControlStates(const ReMIDILoadedPreset &loadedPreset, ReMIDIControlState *controlStatesBuffer)
    {
        for (size_t i = 0; i < loadedPreset.preset.controlCount; ++i)
        {
            EEPROM.get(loadedPreset.controlStatesAddress + i * sizeof(ReMIDIControlState), controlStatesBuffer[i]);
        }
    }

    int getNextPresetAddressFor(ReMIDIPreset &preset)
    {
        ReMIDIPresetListHeader header = ensurePresetList();

        // Find the end of the preset list
        int lastPresetAddress = -1;
        int presetAddress = header.firstPresetAddress;

        while (presetAddress != -1)
        {
            lastPresetAddress = presetAddress;

            ReMIDIPreset currentPreset;
            EEPROM.get(presetAddress, currentPreset);

            // check if there is already a preset with the same PC number
            if (currentPreset.pcNumber == preset.pcNumber)
            {
                // if the PC number is the same, we can overwrite the preset at the same address, but only if the control count is the same,
                // otherwise we would have to shift all subsequent presets in EEPROM, which is not ideal
                if (currentPreset.controlCount == preset.controlCount)
                {
                    preset.nextPresetAddress = currentPreset.nextPresetAddress; // keep the same next preset address
                    return presetAddress;
                }

                // TODO: handle different control count for the same pc number
            }

            presetAddress = currentPreset.nextPresetAddress;
        }

        // we don't replace any existing preset, so we need to add the new preset to the end of the list
        // the new preset will be the last one in the list, so its nextPresetAddress should be -1
        preset.nextPresetAddress = -1;

        // Now lastPresetAddress is the address of the last preset in the list, or -1 if the list is empty

        if (lastPresetAddress == -1)
        {
            // The list is empty, so we can store the new preset right after the header
            int newPresetAddress = sizeof(ReMIDIPresetListHeader);
            header.firstPresetAddress = newPresetAddress;
            EEPROM.put(0, header);

            return newPresetAddress;
        }

        // The list is not empty, so we need to link the last preset to the new preset
        ReMIDIPreset lastPreset;
        EEPROM.get(lastPresetAddress, lastPreset);

        int nextPresetAddress = lastPresetAddress + sizeof(ReMIDIPreset) + lastPreset.controlCount * sizeof(ReMIDIControlState);

        if (nextPresetAddress + sizeof(ReMIDIPreset) + preset.controlCount * sizeof(ReMIDIControlState) > EEPROM.length())
        {
            // not enough space in EEPROM to store the new preset
            return -1;
        }

        // update the last preset's nextPresetAddress to point to the new preset
        lastPreset.nextPresetAddress = nextPresetAddress;
        EEPROM.put(lastPresetAddress, lastPreset);

        return nextPresetAddress;
    }

    bool storePreset(ReMIDIPreset &preset, const ReMIDIControlState *controlStates)
    {
        if (!isPresetValid(preset))
        {
            return false;
        }

        int nextPresetAddress = getNextPresetAddressFor(preset);

        // check for eeprom overflow
        if (nextPresetAddress == -1)
        {
            // not enough space in EEPROM to store the new preset
            return false;
        }

        // Store the new preset at newPresetAddress
        EEPROM.put(nextPresetAddress, preset);

        // copy the control states immediately after the preset struct
        int controlStatesAddress = nextPresetAddress + sizeof(ReMIDIPreset);
        for (size_t i = 0; i < preset.controlCount; ++i)
        {
            EEPROM.put(controlStatesAddress + i * sizeof(ReMIDIControlState), controlStates[i]);
        }

        return true;
    }
}