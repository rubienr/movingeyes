#pragma once

#include <Arduino.h>
#include <inttypes.h>

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

enum class KeyType : uint8_t
{
    A,
    B,
    C,
    D,
    E,
    F,
    X,
    Y,
    Z,
    __KeysCount,
    Void
};

//--------------------------------------------------------------------------------------------------

String stringFromKeyType(const KeyType &kt);

//--------------------------------------------------------------------------------------------------

enum class KeyStateType : uint8_t
{
    Pressed,  // 0, LOW
    Released, // 1, HIGH
    Undefined
};

//--------------------------------------------------------------------------------------------------

String stringFromKeyStateType(const KeyStateType &ks);

//--------------------------------------------------------------------------------------------------

constexpr KeyType keyTypeFromUint8(uint8_t kt) { return static_cast<KeyType>(kt); }

//--------------------------------------------------------------------------------------------------

constexpr uint8_t uint8FromKeyType(Funduino::KeyType k) { return static_cast<uint8_t>(k); }

//--------------------------------------------------------------------------------------------------

constexpr uint8_t getKeyTypesCount() { return static_cast<uint8_t>(KeyType::__KeysCount); }

//--------------------------------------------------------------------------------------------------

constexpr KeyStateType keyStateFromUint8(uint8_t state) { return static_cast<KeyStateType>(state); }

//--------------------------------------------------------------------------------------------------

constexpr uint8_t uint8FromKeyState(KeyStateType kst) { return static_cast<uint8_t>(kst); }

} // namespace Funduino