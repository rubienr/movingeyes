#pragma once

#include "Key.h"
#include "ShieldState.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <inttypes.h>

namespace funduino {

//--------------------------------------------------------------------------------------------------

enum class KeyEventType : uint8_t {
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

struct ShieldEvent {
    KeyType key;
    KeyEventType event;
    ShieldState shield_state;
};

//--------------------------------------------------------------------------------------------------

struct ShieldEventHelper {
    static void print(const ShieldEvent &shield_event, const String & = "ShieldEventHelper::print: ");
    static void println(const ShieldEvent &shield_event, const String & = "ShieldEventHelper::print: ");
};

} // namespace funduino