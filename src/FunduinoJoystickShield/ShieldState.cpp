#include "ShieldState.h"

#include <Arduino.h>

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

bool ShieldState::KeysState::operator==(const Funduino::ShieldState::KeysState &other) const
{
    return a == other.a && b == other.b && c == other.c && d == other.d && e == other.e &&
           f == other.f;
}

//--------------------------------------------------------------------------------------------------

bool ShieldState::KeysState::operator!=(const Funduino::ShieldState::KeysState &other) const
{
    return !this->operator==(other);
}

//--------------------------------------------------------------------------------------------------

bool ShieldState::Joystick::operator==(const Funduino::ShieldState::Joystick &other) const
{
    return x == other.x && y == other.y && z == other.z;
}

//--------------------------------------------------------------------------------------------------

bool ShieldState::Joystick::operator!=(const Funduino::ShieldState::Joystick &other) const
{
    return !this->operator==(other);
}

//--------------------------------------------------------------------------------------------------

bool ShieldState::operator==(const Funduino::ShieldState &other) const
{
    return keys.operator==(other.keys) && joystick.operator==(other.joystick);
}

//--------------------------------------------------------------------------------------------------

bool ShieldState::operator!=(const Funduino::ShieldState &other) const
{
    return !this->operator==(other);
}


//--------------------------------------------------------------------------------------------------

void ShieldStateHelper::print(const ShieldState &state)
{
    String s("ShieldStateHelper::print:");

    s.concat(" A=");
    s.concat(uint8FromKeyState(state.keys.a));
    s.concat(" B=");
    s.concat(uint8FromKeyState(state.keys.b));
    s.concat(" C=");
    s.concat(uint8FromKeyState(state.keys.c));
    s.concat(" D=");
    s.concat(uint8FromKeyState(state.keys.d));
    s.concat(" E=");
    s.concat(uint8FromKeyState(state.keys.e));
    s.concat(" F=");
    s.concat(uint8FromKeyState(state.keys.f));

    s.concat(" Z=");
    s.concat(uint8FromKeyState(state.joystick.z));
    s.concat(" X:(min, center, max, value)=(");
    s.concat(state.joystick.x.min);
    s.concat(", ");
    s.concat(state.joystick.x.center);
    s.concat(", ");
    s.concat(state.joystick.x.max);
    s.concat(", ");
    s.concat(state.joystick.x.value);
    s.concat(")");
    s.concat(" Y:(min, center, max, value)=(");
    s.concat(state.joystick.y.min);
    s.concat(", ");
    s.concat(state.joystick.y.center);
    s.concat(", ");
    s.concat(state.joystick.y.max);
    s.concat(", ");
    s.concat(state.joystick.y.value);
    s.concat(")");

    Serial.println(s.c_str());
}

//--------------------------------------------------------------------------------------------------

bool Potentiometer::operator==(const Potentiometer &other) const { return value == other.value; }

//--------------------------------------------------------------------------------------------------

bool Potentiometer::operator!=(const Potentiometer &other) const { return !operator==(other); }

//--------------------------------------------------------------------------------------------------

void Potentiometer::update(const uint16_t &new_value)
{
    value = new_value;
    max = max(max, new_value);
    min = min(min, new_value);
}

//--------------------------------------------------------------------------------------------------

Potentiometer &Potentiometer::operator=(const Potentiometer &rhs)
{
    if(this == &rhs)
    {
        return *this;
    }

    value = rhs.value;
    min = min(min, rhs.min);
    max = max(max, rhs.max);
    // center is skipped

    return *this;
}

//--------------------------------------------------------------------------------------------------

float Potentiometer::getNormalizedValue() const
{
    uint16_t range = max - min;

    return static_cast<float>(value) / range;
}

} // namespace Funduino
