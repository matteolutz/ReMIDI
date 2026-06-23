#ifndef REMIDI_HPP
#define REMIDI_HPP

namespace remidi
{
    template <class Transport, class Settings, class Platform>
    inline void ReMIDI<Transport, Settings, Platform>::begin(MIDI_NAMESPACE::Channel inChannel)
    {
        ensurePresetList();

        m_Midi.begin(inChannel);

        // initialize controls
        for (size_t i = 0; i < m_ControlList.size; ++i)
        {
            ReMIDIControl &control = m_ControlList.controls[i];
            control.begin();
        }

        // initialize learn button pin if it's defined
        if (m_LearnButton.isValid())
        {
            m_LearnButton.begin();

            // when the learn button is held during startup, we clear the preset list in EEPROM
            if (m_LearnButton.isPressed())
            {
                initializePresetList();
            }
        }
    }

    template <class Transport, class Settings, class Platform>
    void ReMIDI<Transport, Settings, Platform>::update()
    {
        // check for new midi messages
        if (m_Midi.read())
        {
            switch (m_Midi.getType())
            {
            case MIDI_NAMESPACE::ProgramChange:
                handleProgramChange();
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

    template <class Transport, class Settings, class Platform>
    void ReMIDI<Transport, Settings, Platform>::learn()
    {
        while (m_LearnButton.isPressed())
        {
            if (!m_Midi.read())
                continue;

            if (m_Midi.getType() != MIDI_NAMESPACE::ProgramChange)
                continue;

            uint8_t pcNumber = m_Midi.getData1();

            ReMIDIControlState controlStates[m_ControlList.size];
            for (size_t i = 0; i < m_ControlList.size; ++i)
            {
                ReMIDIControl &control = m_ControlList.controls[i];
                controlStates[i].controlId = control.id;
                controlStates[i].state = control.getState();
            }

            ReMIDIPreset preset;
            preset.pcNumber = pcNumber;
            preset.controlCount = m_ControlList.size;

            storePreset(preset, controlStates);

            // Wait for the learn button to be released before allowing another preset to be learned
            while (m_LearnButton.isPressed())
                ;
        }
    }

    template <class Transport, class Settings, class Platform>
    void ReMIDI<Transport, Settings, Platform>::handleProgramChange()
    {
        uint8_t pcNumber = m_Midi.getData1();
        ReMIDILoadedPreset preset = findPresetForPCNumber(pcNumber);

        if (!isPresetValid(preset.preset))
        {
            return;
        }

        applyPreset(preset);
    }

    template <class Transport, class Settings, class Platform>
    void ReMIDI<Transport, Settings, Platform>::applyPreset(const ReMIDILoadedPreset &preset)
    {
        ReMIDIControlState controlStates[preset.preset.controlCount];
        loadPresetControlStates(preset, controlStates);

        for (size_t i = 0; i < preset.preset.controlCount; ++i)
        {
            ReMIDIControlState &controlState = controlStates[i];

            ReMIDIControl *control = m_ControlList.findControlById(controlState.controlId);
            if (control == nullptr)
            {
                continue;
            }

            control->applyState(controlState.state);
        }
    }
}

#endif // REMIDI_HPP