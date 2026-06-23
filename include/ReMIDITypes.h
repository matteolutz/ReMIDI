#ifndef REMIDI_TYPES_H
#define REMIDI_TYPES_H

/* ------------------------------ Integer Types ----------------------------- */
#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef __SIZE_TYPE__ size_t;
#endif

#endif // REMIDI_TYPES_H