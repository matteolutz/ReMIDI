#include "ReMIDI.h"

namespace remidi
{
    void ReMIDI::begin()
    {
        // init debug logger
        REMIDI_DEBUG_BEGIN();
        REMIDI_DEBUG_LOG("Initializing ReMIDI...");

        REMIDI_DEBUG_LOG("Ensuring preset list is valid...");
        ensurePresetList();

        REMIDI_DEBUG_LOG("Initializing controls...");
        // initialize controls
        for (size_t i = 0; i < m_ControlList.size; ++i)
        {
            const ReMIDIControl &control = m_ControlList.controls[i];
            control.begin();
        }

        // initialize learn button pin if it's defined
        if (m_LearnButton.isValid())
        {
            REMIDI_DEBUG_LOG("Initializing learn button...");
            m_LearnButton.begin();

#if ARDUINO
            REMIDI_DEBUG_LOG("Waiting for 1s to allow user to hold learn button for clearing preset list...");
            delay(1000);
#endif

            // when the learn button is held during startup, we clear the preset list in EEPROM
            if (m_LearnButton.isPressed())
            {
                REMIDI_DEBUG_LOG("Learn button is held during startup, clearing preset list...");
                initializePresetList();

                REMIDI_DEBUG_LOG("Preset list cleared. Please release the learn button.");
                while (m_LearnButton.isPressed())
                    ;
            }
        }

        REMIDI_DEBUG_LOG("There are ", getPresetCount(), " presets stored in EEPROM.");
        REMIDI_DEBUG_LOG("Done! Let's rock n' roll!");
    }

    void ReMIDI::update()
    {
        ReMIDIMessage message = m_ReadMidiMessage();
        // check for new midi messages
        if (message.isValid())
        {
            switch (message.type)
            {
            case ReMIDIMessageType::ProgramChange:
                handleProgramChange(message.data1);
                break;
            default:
                break;
            }
        }

        // check for learn button press
        if (m_LearnButton.isValid() && m_LearnButton.isPressed())
        {
            learn();
        }
    }

    void ReMIDI::learn()
    {
        REMIDI_DEBUG_LOG("Learn button pressed, waiting for ProgramChange message...");
        while (m_LearnButton.isPressed())
        {
            ReMIDIMessage message = m_ReadMidiMessage();
            if (!message.isValid())
                continue;

            if (message.type != ReMIDIMessageType::ProgramChange)
                continue;

            uint8_t pcNumber = message.data1;

            REMIDI_DEBUG_LOG("Learning preset for ProgramChange: ", pcNumber);

            ReMIDIControlState controlStates[m_ControlList.size];
            for (size_t i = 0; i < m_ControlList.size; ++i)
            {
                const ReMIDIControl &control = m_ControlList.controls[i];
                controlStates[i].controlId = control.id;
                controlStates[i].state = control.getState();

                REMIDI_DEBUG_LOG("\tControl ", control.id, " state: ", controlStates[i].state);
            }

            ReMIDIPreset preset;
            preset.pcNumber = pcNumber;
            preset.controlCount = m_ControlList.size;

            storePreset(preset, controlStates);

            // Wait for the learn button to be released before allowing another preset to be learned
            while (m_LearnButton.isPressed())
                ;
        }

        REMIDI_DEBUG_LOG("Done learning presets!");
    }

    void ReMIDI::handleProgramChange(uint8_t pcNumber)
    {
        REMIDI_DEBUG_LOG("Received ProgramChange: ", pcNumber);

        ReMIDILoadedPreset preset = findPresetForPCNumber(pcNumber);

        if (!isPresetValid(preset.preset))
        {
            return;
        }

        applyPreset(preset);
    }

    void ReMIDI::applyPreset(const ReMIDILoadedPreset &preset)
    {
        REMIDI_DEBUG_LOG("Applying preset for ProgramChange: ", preset.preset.pcNumber);

        ReMIDIControlState controlStates[preset.preset.controlCount];
        loadPresetControlStates(preset, controlStates);

        for (size_t i = 0; i < preset.preset.controlCount; ++i)
        {
            const ReMIDIControlState &controlState = controlStates[i];

            ReMIDIControl const *control = m_ControlList.findControlById(controlState.controlId);
            if (control == nullptr)
            {
                continue;
            }

            REMIDI_DEBUG_LOG("\tApplying control ", controlState.controlId, ": ", controlState.state);
            control->applyState(controlState.state);
        }
    }
}