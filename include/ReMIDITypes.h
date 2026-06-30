#ifndef REMIDI_TYPES_H
#define REMIDI_TYPES_H

/* ------------------------------ Integer Types ----------------------------- */
#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef __SIZE_TYPE__ size_t;

#define NOT_A_PIN 0
#endif

#ifdef REMIDI_DEBUG

#ifndef REMIDI_DEBUG_SERIAL
#error "REMIDI_DEBUG_SERIAL must be defined when REMIDI_DEBUG is defined"
#endif

#ifndef REMIDI_DEBUG_BAUD
#define REMIDI_DEBUG_BAUD 115200
#endif

template <typename T>
inline void remidi__log_impl(const T &value)
{
    REMIDI_DEBUG_SERIAL.print(value);
}

template <typename T, typename... Args>
inline void remidi__log_impl(const T &value, const Args &...args)
{
    REMIDI_DEBUG_SERIAL.print(value);
    remidi__log_impl(args...);
}

#define REMIDI_DEBUG_BEGIN()                          \
    do                                                \
    {                                                 \
        REMIDI_DEBUG_SERIAL.begin(REMIDI_DEBUG_BAUD); \
        while (!REMIDI_DEBUG_SERIAL)                  \
            ;                                         \
    } while (0)

#define REMIDI_DEBUG_LOG(...)          \
    do                                 \
    {                                  \
        Serial.print("[ReMIDI] ");     \
        remidi__log_impl(__VA_ARGS__); \
        Serial.println();              \
    } while (0)

#define REMIDI_DEBUG_TRACE(...)          \
    do                                   \
    {                                    \
        Serial.print("[ReMIDI TRACE] "); \
        remidi__log_impl(__VA_ARGS__);   \
        Serial.println();                \
    } while (0)

#else
#define REMIDI_DEBUG_BEGIN()
#define REMIDI_DEBUG_LOG(...)
#define REMIDI_DEBUG_TRACE(...)
#endif

#endif // REMIDI_TYPES_H