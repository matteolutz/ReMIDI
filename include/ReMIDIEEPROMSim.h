#ifndef REMIDI_EEPROM_SIM_H
#define REMIDI_EEPROM_SIM_H

#include "ReMIDITypes.h"

namespace remidi
{

    class ReMIDIEEPROMSimClass
    {
    public:
        ReMIDIEEPROMSimClass(uint8_t *data, uint16_t size) : m_EepromData(data), m_EepromSize(size) {}

        ReMIDIEEPROMSimClass(uint16_t size = 1024) : m_EepromSize(size)
        {
            m_EepromData = new uint8_t[size];
        }

        uint16_t length() const
        {
            return m_EepromSize;
        }

        template <typename T>
        T &get(int idx, T &t)
        {
            int e = idx;
            uint8_t *ptr = (uint8_t *)&t;
            for (int count = sizeof(T); count; --count, ++e)
                *ptr++ = m_EepromData[e];
            return t;
        }

        template <typename T>
        const T &put(int idx, const T &t)
        {
            int e = idx;
            const uint8_t *ptr = (const uint8_t *)&t;
            for (int count = sizeof(T); count; --count, ++e)
                m_EepromData[e] = *ptr++;
            return t;
        }

    private:
        uint8_t *m_EepromData;
        uint16_t m_EepromSize;
    };
}

// The user has to define this variable in their own code
extern remidi::ReMIDIEEPROMSimClass EEPROM;

#endif // REMIDI_EEPROM_SIM_H