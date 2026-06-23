#include "ReMIDIControl.h"

namespace remidi
{

    ReMIDIControl createSwitchControl(uint8_t id, ReMIDISwitchControlConfig config)
    {
        ReMIDIControl control;
        control.id = id;
        control.type = ReMIDIControlType::Switch;

        control.config.switchConfig = config;

        return control;
    }

}