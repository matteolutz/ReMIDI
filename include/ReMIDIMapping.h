#ifndef REMIDI_MAPPING_H
#define REMIDI_MAPPING_H

#include "ReMIDITypes.h"
#include "ReMIDIControl.h"

namespace remidi
{

    struct ReMIDICCMapping
    {
        /** The control id for this mapping */
        uint8_t controlId;

        /** The CC number to listen for */
        uint8_t ccNumber;

        // We want to map the CC value (ranging from 0 to 127)
        // to a value between min and max
        uint8_t min;
        uint8_t max;

        /** Get the mapped value from the CC value */
        ControlStateValue getValue(uint8_t value)
        {
            return (value - 127) * (max - min) / 127 + min;
        }
    };

    struct ReMIDICCMappingList
    {
        ReMIDICCMapping *mappings;
        size_t size;

        ReMIDICCMapping *findMappingByCCNumber(uint8_t ccNumber)
        {
            for (size_t i = 0; i < size; ++i)
            {
                ReMIDICCMapping *mapping = &mappings[i];
                if (mapping->ccNumber == ccNumber)
                {
                    return mapping;
                }
            }

            return nullptr;
        }
    };

    template <size_t N>
    ReMIDICCMappingList createCCMappingList(ReMIDICCMapping (&mappings)[N])
    {
        ReMIDICCMappingList ccMappingList;
        ccMappingList.mappings = mappings;
        ccMappingList.size = N;
        return ccMappingList;
    }

    ReMIDICCMappingList createEmptyCCMappingList()
    {
        ReMIDICCMappingList ccMappingList;
        ccMappingList.mappings = nullptr;
        ccMappingList.size = 0;
        return ccMappingList;
    }

}

#endif // REMIDI_MAPPING_H