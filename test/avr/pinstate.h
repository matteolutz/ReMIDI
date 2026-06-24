#ifndef PINSTATE_H
#define PINSTATE_H

#include <Arduino.h>

/**
 * This struct represents the state of a digital pin on an AVR microcontroller.
 */
struct DigitalPinState
{
    /**
     * Whether the pin state is valid.
     * Will be set to `false`, if for example the provided pin is not valid
     */
    bool isValid;

    /**
     * This means different things, depending on the mode of the pin:
     * - `INPUT | INPUT_PULLUP`   : the value of the pin input register (so same as `digitalRead(..)`)
     * - `OUTPUT`                 : the value of the pin output register (so the value `digitalWrite(..)` sets)
     */
    bool isHigh;

    /** The mode of the pin, i.e. INPUT, INPUT_PULLUP or OUTPUT */
    int mode;

    /* ----------------------- Convenience helper methods ----------------------- */
    inline bool isOutput()
    {
        return mode == OUTPUT;
    }

    inline bool isInput()
    {
        return mode == INPUT;
    }

    inline bool isInputPullup()
    {
        return mode == INPUT_PULLUP;
    }

    inline bool isInputOrInputPullup()
    {
        return mode == INPUT || mode == INPUT_PULLUP;
    }

    bool inline isModeAndState(int _mode, bool _isHigh)
    {
        return mode == _mode && isHigh == _isHigh;
    }

    inline bool isInputAndHigh()
    {
        return isModeAndState(INPUT, true);
    }

    inline bool isInputAndLow()
    {
        return isModeAndState(INPUT, false);
    }

    inline bool isInputPullupAndLow()
    {
        return isModeAndState(INPUT_PULLUP, false);
    }

    inline bool isInputPullupAndHigh()
    {
        return isModeAndState(INPUT_PULLUP, true);
    }

    inline bool isOutputAndHigh()
    {
        return isModeAndState(OUTPUT, true);
    }

    inline bool isOutputAndLow()
    {
        return isModeAndState(OUTPUT, false);
    }
};

constexpr DigitalPinState INVALID_DIGITAL_PIN_STATE = {.isValid = false, .isHigh = false, .mode = -1};

DigitalPinState getDigitalPinState(uint8_t pin)
{
    uint8_t portBitMask = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);

    if (port == NOT_A_PIN)
    {
        return INVALID_DIGITAL_PIN_STATE;
    }

    bool isOutput = (*portModeRegister(port) & portBitMask) != 0;

    if (isOutput)
    {
        bool outputState = (*portOutputRegister(port) & portBitMask) != 0;
        return DigitalPinState{
            .isValid = true,
            .isHigh = outputState,
            .mode = OUTPUT,
        };
    }

    bool inputState = digitalRead(pin) == HIGH; // kinda wasteful, but we can't call turnOffPWM directly
    bool isPullup = (*portOutputRegister(port) & portBitMask) != 0;
    return DigitalPinState{
        .isValid = true,
        .isHigh = inputState,
        .mode = isPullup ? INPUT_PULLUP : INPUT,
    };
}

#endif // PINSTATE_H