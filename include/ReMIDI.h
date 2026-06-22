#ifndef REMIDI_H
#define REMIDI_H

#include <MIDI.h>

#include "ReMIDIControl.h"
#include "ReMIDIPresets.h"

namespace remidi
{

    template <class Transport, class _Settings = MIDI_NAMESPACE::DefaultSettings, class _Platform = MIDI_NAMESPACE::DefaultPlatform>
    class ReMIDI
    {
    public:
        ReMIDI(MIDI_NAMESPACE::MidiInterface<Transport, _Settings, _Platform> &midiInterface, ReMIDIControlList controlList, uint8_t learnButton = NOT_A_PIN)
            : m_Midi(midiInterface), m_ControlList(controlList), m_LearnButton(learnButton)
        {
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

        uint8_t m_LearnButton;
    };

}

#include <ReMIDI.hpp>

#endif // REMIDI_H