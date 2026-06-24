#ifndef REMIDI_CONTROL_H
#define REMIDI_CONTROL_H

#include "ReMIDITypes.h"

namespace remidi
{

    typedef uint8_t ControlStateValue;

    enum class ReMIDIControlType : uint8_t
    {
        Switch
    };

    struct ReMIDISwitchControlConfig
    {
        /** The input pin the actual switch is connected to */
        uint8_t inputPin;
        bool inputPullup;

        /** The output pin, we want to control */
        uint8_t outputPin;
        bool outputActiveLow;

        void begin() const
        {
#if ARDUINO
            pinMode(inputPin, inputPullup ? INPUT_PULLUP : INPUT);

            pinMode(outputPin, OUTPUT);
            digitalWrite(outputPin, LOW);
#endif
        }

        void applyState(ControlStateValue state) const
        {
#if ARDUINO
            digitalWrite(outputPin, (!outputActiveLow && state) || (outputActiveLow && !state) ? HIGH : LOW);
#endif
        }

        ControlStateValue getState() const
        {
#if ARDUINO
            return digitalRead(inputPin) == (inputPullup ? LOW : HIGH) ? 1 : 0;
#else
            return 0;
#endif
        }
    };

    struct ReMIDIControl
    {
        /**
         * Id of the control
         * An id of -1 means this control is invalid, and should not be used
         * */
        int8_t id = -1;

        /** Type of the control */
        ReMIDIControlType type;

        /**
         * Configuration for the control.
         */
        union
        {
            ReMIDISwitchControlConfig switchConfig;
            // Future control types can be added here with their respective configurations
        } config;

        void begin() const
        {
            switch (type)
            {
            case ReMIDIControlType::Switch:
                config.switchConfig.begin();
                break;
            default:
                break;
            }
        }

        void applyState(ControlStateValue state) const
        {
            switch (type)
            {
            case ReMIDIControlType::Switch:
                config.switchConfig.applyState(state);
                break;
            default:
                break;
            }
        }

        ControlStateValue getState() const
        {
            switch (type)
            {
            case ReMIDIControlType::Switch:
                return config.switchConfig.getState();
            default:
                return 0;
            }
        }
    };

    struct __attribute__((packed)) ReMIDIControlState
    {
        /** The ID of the control */
        uint8_t controlId;

        /**
         * The state of the control
         * This is a uint8_t to allow for future expansion of control types,
         * for example, an encoder control might use this to store the current value of the encoder, while a switch control might just use 0 and 1 for off and on states.
         */
        ControlStateValue state;
    };

    struct ReMIDIControlList
    {
        ReMIDIControl *controls;
        size_t size;

        ReMIDIControl *findControlById(uint8_t controlId)
        {
            for (size_t i = 0; i < size; ++i)
            {
                ReMIDIControl *control = &controls[i];
                if (control->id == controlId)
                {
                    return control;
                }
            }

            return nullptr;
        }
    };

    template <size_t N>
    ReMIDIControlList createControlList(ReMIDIControl (&controls)[N])
    {
        ReMIDIControlList controlList;
        controlList.controls = controls;
        controlList.size = N;
        return controlList;
    }

    /* --------- Helper functions to create different types of controls --------- */
    ReMIDIControl createSwitchControl(uint8_t id, ReMIDISwitchControlConfig config);
}

#endif // REMIDI_CONTROL_H