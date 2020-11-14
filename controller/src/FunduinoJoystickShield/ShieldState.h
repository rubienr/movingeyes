#pragma once

#include "Key.h"
#include <HardwareSerial.h>

namespace funduino {

//--------------------------------------------------------------------------------------------------

struct Potentiometer {
    uint16_t min{ 1023 }, max{ 1 }, center{ 511 }, value = { 511 };

    void update(const uint16_t &new_value);

    float getNormalizedValue() const;

    /**
     * Compares only the potentiometer value no min/center/max values!
     */
    bool operator==(const Potentiometer &other) const;
    bool operator!=(const Potentiometer &other) const;

    void copyFrom(const Potentiometer &other);
};

//--------------------------------------------------------------------------------------------------

struct ShieldState {
    struct KeysState {
        KeyStateType a{ KeyStateType::Undefined }, b{ KeyStateType::Undefined },
        c{ KeyStateType::Undefined }, d{ KeyStateType::Undefined }, e{ KeyStateType::Undefined },
        f{ KeyStateType::Undefined };

        bool operator==(const KeysState &other) const;
        bool operator!=(const KeysState &other) const;
    };

    struct Joystick {
        KeyStateType z{ KeyStateType::Undefined };
        Potentiometer x, y;

        bool operator==(const Joystick &other) const;
        bool operator!=(const Joystick &other) const;
    };

    KeysState keys;
    Joystick joystick;

    bool operator==(const ShieldState &other) const;
    bool operator!=(const ShieldState &other) const;

    ShieldState &operator=(const ShieldState &rhs);
};

//--------------------------------------------------------------------------------------------------

struct ShieldStateHelper {
    static void print(const ShieldState &state, const String &prefix = "ShieldStateHelper::print: ");
    static void println(const ShieldState &state, const String &prefix = "ShieldStateHelper::print: ");
};

} // namespace funduino
