#ifndef REMIDI_H
#define REMIDI_H

#include <MIDI.h>

#include "ReMIDIControl.h"
#include "ReMIDIPresets.h"

#include "ReMIDIButton.h"

namespace remidi
{

    template <class Transport, class _Settings = MIDI_NAMESPACE::DefaultSettings, class _Platform = MIDI_NAMESPACE::DefaultPlatform>
    class ReMIDI
    {
    public:
        ReMIDI(MIDI_NAMESPACE::MidiInterface<Transport, _Settings, _Platform> &midiInterface, ReMIDIControlList controlList)
            : m_Midi(midiInterface), m_ControlList(controlList)
        {
        }

        inline void setLearnButton(uint8_t pin, bool pullup = false)
        {
            m_LearnButton = ReMIDIButton(pin, pullup);
        }

        inline void begin(MIDI_NAMESPACE::Channel inChannel = 1);

        void update();

    private:
        void handleProgramChange();

        void applyPreset(const ReMIDILoadedPreset &preset);

        void learn();

    private:
        MIDI_NAMESPACE::MidiInterface<Transport, _Settings, _Platform> &m_Midi;

        ReMIDIControlList m_ControlList;

        ReMIDIButton m_LearnButton = NOT_A_PIN;
    };

}

#include <ReMIDI.hpp>

#endif // REMIDI_H