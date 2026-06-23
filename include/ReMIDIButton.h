#ifndef REMIDI_BUTTON_H
#define REMIDI_BUTTON_H

#include "ReMIDITypes.h"

namespace remidi
{
    class ReMIDIButton
    {
    public:
        ReMIDIButton(uint8_t pin, bool pullup = false)
            : m_Pin(pin), m_Pullup(pullup)
        {
        }

        void begin()
        {
#if ARDUINO
            pinMode(m_Pin, m_Pullup ? INPUT_PULLUP : INPUT);
#endif
        }

        bool isPressed() const
        {
#if ARDUINO
            return digitalRead(m_Pin) == (m_Pullup ? LOW : HIGH);
#else
            return false;
#endif
        }

        bool isValid() const
        {
#if ARDUINO
            return m_Pin != NOT_A_PIN;
#else
            return false;
#endif
        }

    private:
        uint8_t m_Pin;
        bool m_Pullup;
    };
}

#endif // REMIDI_BUTTON_H