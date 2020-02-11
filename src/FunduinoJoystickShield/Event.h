#pragma once

#include "Key.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <inttypes.h>

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

enum class KeyEventType : uint8_t
{
    Pressed,  // 0
    Released, // 1
    Changed,  // 2
    None      // 3
};

//--------------------------------------------------------------------------------------------------

constexpr uint8_t uint8FromEventType(const KeyEventType &et) { return static_cast<uint8_t>(et); }

//--------------------------------------------------------------------------------------------------

String stringFromEventType(const KeyEventType &et);

//--------------------------------------------------------------------------------------------------

struct ShieldEvent
{
    static constexpr uint8_t KEYS_COUNT{ getKeyTypesCount() };
    KeyEventType key_event[KEYS_COUNT]{ KeyEventType::None, KeyEventType::None, KeyEventType::None,
                                        KeyEventType::None, KeyEventType::None, KeyEventType::None,
                                        KeyEventType::None, KeyEventType::None, KeyEventType::None };
    KeyEventType key_state[KEYS_COUNT]{ KeyEventType::None, KeyEventType::None, KeyEventType::None,
                                        KeyEventType::None, KeyEventType::None, KeyEventType::None,
                                        KeyEventType::None, KeyEventType::None, KeyEventType::None };
};

//--------------------------------------------------------------------------------------------------

struct ShieldEventHelper
{
    static void print(const ShieldEvent &shield_event);
};

} // namespace Funduino