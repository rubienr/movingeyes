#pragma once

#include "Key.h"
#include <HardwareSerial.h>

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

struct Potentiometer
{
    uint16_t min{ 0xffff }, max{ 1 }, center{ 0xff }, value = { 0xff };

    void update(const uint16_t &new_value);

    float getNormalizedValue() const;

    /**
     * Compares only the potentiometer value no min/center/max values!
     */
    bool operator==(const Potentiometer &other) const;
    bool operator!=(const Potentiometer &other) const;

    /**
     * Assigns current value except
     *   - center position (remains untouched)
     *   - min (the minimum of both is taken)
     *   - max (the maximum of both is taken)
     */
    Potentiometer &operator=(const Potentiometer &rhs);
};

//--------------------------------------------------------------------------------------------------

struct ShieldState
{
    struct KeysState
    {
        KeyStateType a, b, c, d, e, f;

        bool operator==(const KeysState &other) const;
        bool operator!=(const KeysState &other) const;
    };

    struct Joystick
    {
        KeyStateType z;
        Potentiometer x, y;

        bool operator==(const Joystick &other) const;
        bool operator!=(const Joystick &other) const;
    };

    KeysState keys;
    Joystick joystick;

    bool operator==(const ShieldState &other) const;
    bool operator!=(const ShieldState &other) const;
};

//--------------------------------------------------------------------------------------------------

struct ShieldStateHelper
{
    static void print(const ShieldState &state);
};

} //  namespace Funduino
