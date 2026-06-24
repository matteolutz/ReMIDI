#ifndef EEPROM_SIM_IMPL_H
#define EEPROM_SIM_IMPL_H

#include <ReMIDIEEPROMSim.h>

#define EEPROM_SIZE 1024

uint8_t eepromData[EEPROM_SIZE];
remidi::ReMIDIEEPROMSimClass EEPROM(eepromData, EEPROM_SIZE);

#endif // EEPROM_SIM_IMPL_H