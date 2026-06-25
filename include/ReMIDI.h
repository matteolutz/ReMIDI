#ifndef REMIDI_H
#define REMIDI_H

#include "ReMIDIControl.h"
#include "ReMIDIPresets.h"

#include "ReMIDIButton.h"

#include "ReMIDIMessage.h"

namespace remidi
{

    class ReMIDI
    {
    public:
        ReMIDI(ReMIDIMessage (*readMidiMessage)(), ReMIDIControlList controlList)
            : m_ReadMidiMessage(readMidiMessage), m_ControlList(controlList)
        {
        }

        inline void setLearnButton(uint8_t pin, bool pullup = false)
        {
            m_LearnButton = ReMIDIButton(pin, pullup);
        }

        void begin();

        void update();

    private:
        void handleProgramChange(uint8_t pcNumber);
        void applyPreset(const ReMIDILoadedPreset &preset);

        void learn();

    private:
        ReMIDIMessage (*m_ReadMidiMessage)();
        ReMIDIControlList m_ControlList;

        ReMIDIButton m_LearnButton = NOT_A_PIN;
    };

}

#endif // REMIDI_H