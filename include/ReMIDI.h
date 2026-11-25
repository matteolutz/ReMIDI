#ifndef REMIDI_H
#define REMIDI_H

#include "ReMIDIControl.h"
#include "ReMIDIPresets.h"
#include "ReMIDIMapping.h"

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
            m_CCMappingList = createEmptyCCMappingList();
        }

        ReMIDI(ReMIDIMessage (*readMidiMessage)(), ReMIDIControlList controlList, ReMIDICCMappingList ccMappingList)
            : m_ReadMidiMessage(readMidiMessage), m_ControlList(controlList), m_CCMappingList(ccMappingList)
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
        void handleControlChange(uint8_t ccNumber, uint8_t ccValue);
        void applyPreset(const ReMIDILoadedPreset &preset);

        void learn();

    private:
        ReMIDIMessage (*m_ReadMidiMessage)();
        ReMIDIControlList m_ControlList;
        ReMIDICCMappingList m_CCMappingList;

        ReMIDIButton m_LearnButton = ReMIDIButton(NOT_A_PIN);
    };

}

#endif // REMIDI_H