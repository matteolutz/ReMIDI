#include "ReMIDIControl.h"

namespace remidi
{

    ReMIDIControl createSwitchControl(uint8_t id, uint8_t inputPin, uint8_t outputPin, bool activeLow)
    {
        ReMIDIControl control;
        control.id = id;
        control.type = ReMIDIControlType::Switch;

        control.config.switchConfig.inputPin = inputPin;
        control.config.switchConfig.outputPin = outputPin;
        control.config.switchConfig.activeLow = activeLow;

        return control;
    }

}