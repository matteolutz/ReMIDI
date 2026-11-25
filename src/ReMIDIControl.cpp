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

    ReMIDIControl createCustomControl(uint8_t id, void (*begin)(), void (*applyState)(ControlStateValue state), ControlStateValue (*getState)())
    {
        ReMIDIControl control;
        control.id = id;
        control.type = ReMIDIControlType::Custom;

        control.config.customConfig = ReMIDICustomControlConfig{.begin = begin, .applyState = applyState, .getState = getState};

        return control;
    }

}