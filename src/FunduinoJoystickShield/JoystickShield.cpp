#include "JoystickShield.h"
#include "ShieldState.h"
#include <Arduino.h>

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

JoystickShield::JoystickShield(const ArduinoPinConfiguration &io_pins) : pins(io_pins) {}

//--------------------------------------------------------------------------------------------------

void JoystickShield::setup()
{
    pinMode(pins.input.keys.a, INPUT);
    pinMode(pins.input.keys.b, INPUT);
    pinMode(pins.input.keys.c, INPUT);
    pinMode(pins.input.keys.d, INPUT);
    pinMode(pins.input.keys.e, INPUT);
    pinMode(pins.input.keys.f, INPUT);

    digitalWrite(pins.input.keys.a, HIGH);
    digitalWrite(pins.input.keys.b, HIGH);
    digitalWrite(pins.input.keys.c, HIGH);
    digitalWrite(pins.input.keys.d, HIGH);
    digitalWrite(pins.input.keys.e, HIGH);
    digitalWrite(pins.input.keys.f, HIGH);

    pinMode(pins.input.joystick.z, INPUT);
    digitalWrite(pins.input.joystick.z, HIGH);

    analogReference(DEFAULT);
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::process()
{
    ShieldState new_state = readState();

    // on key event
    bool has_changed{ state.keys != new_state.keys };

    // on fuzzy x/y change
    if(!isWithinDelta(new_state.joystick.x.value, state.joystick.x.value, min_potentiometer_readout_delta) ||
       !isWithinDelta(new_state.joystick.y.value, state.joystick.y.value, min_potentiometer_readout_delta))
    {
        has_changed = true;
    }

    // on z-key event
    if(state.joystick.z != new_state.joystick.z)
    {
        has_changed = true;
    }

    // update state and trigger events
    if(has_changed)
    {
        // Serial.println("JoystickShield::process: state changed");
        // ShieldStateHelper::print(state);
        // ShieldStateHelper::print(new_state);

        if(event_receiver != nullptr)
        {
            ShieldEvent event{ getEvent(new_state) };
            // ShieldEventHelper::print(event);
            state = new_state;
            event_receiver->take(event);
        }
        else
        {
            state = new_state;
        }
    }

    return has_changed;
}

//--------------------------------------------------------------------------------------------------

ShieldState JoystickShield::readState()
{
    ShieldState current_state{ { keyStateFromUint8(digitalRead(pins.input.keys.a)),
                                 keyStateFromUint8(digitalRead(pins.input.keys.b)),
                                 keyStateFromUint8(digitalRead(pins.input.keys.c)),
                                 keyStateFromUint8(digitalRead(pins.input.keys.d)),
                                 keyStateFromUint8(digitalRead(pins.input.keys.e)),
                                 keyStateFromUint8(digitalRead(pins.input.keys.f)) },
                               {
                               keyStateFromUint8(digitalRead(pins.input.joystick.z)),
                               // joystick.x
                               // joystick.y
                               } };

    current_state.joystick.x = state.joystick.x;
    current_state.joystick.y = state.joystick.y;

    current_state.joystick.x.update(static_cast<uint16_t>(analogRead(pins.input.joystick.x)));
    current_state.joystick.y.update(static_cast<uint16_t>(analogRead(pins.input.joystick.y)));

    // TODO: calibrate x/y center position

    return current_state;
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::setEventReceiver(EventReceiver *receiver)
{
    if(event_receiver != receiver)
    {
        event_receiver = receiver;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------

ShieldEvent JoystickShield::getEvent(const ShieldState &new_state)
{
    ShieldEvent event;

    addKeyEvent(event, KeyType::A, state.keys.a, new_state.keys.a);
    addKeyEvent(event, KeyType::B, state.keys.b, new_state.keys.b);
    addKeyEvent(event, KeyType::C, state.keys.c, new_state.keys.c);
    addKeyEvent(event, KeyType::D, state.keys.d, new_state.keys.d);
    addKeyEvent(event, KeyType::E, state.keys.e, new_state.keys.e);
    addKeyEvent(event, KeyType::F, state.keys.f, new_state.keys.f);


    addJoystickEvent(event, KeyType::X, state.joystick.x.value, new_state.joystick.x.value,
                     min_potentiometer_readout_delta);
    addJoystickEvent(event, KeyType::Y, state.joystick.y.value, new_state.joystick.y.value,
                     min_potentiometer_readout_delta);
    addKeyEvent(event, KeyType::Z, state.joystick.z, new_state.joystick.z);

    return event;
}

//--------------------------------------------------------------------------------------------------

KeyEventType JoystickShield::eventTypeFromKeyState(KeyStateType ks)
{
    if(ks == KeyStateType::Pressed)
    {
        return KeyEventType::Pressed;
    }
    else if(ks == KeyStateType::Released)
    {
        return KeyEventType::Released;
    }
    else if(ks == KeyStateType::Undefined)
    {
        return KeyEventType::None;
    }

    return KeyEventType::None;
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::addKeyEvent(ShieldEvent &event, KeyType key, KeyStateType current_key_state, KeyStateType new_key_state)
{
    event.key_state[uint8FromKeyType(key)] = eventTypeFromKeyState(new_key_state);

    if(current_key_state == new_key_state)
    {
        return false;
    }
    else
    {
        event.key_event[uint8FromKeyType(key)] = eventTypeFromKeyState(new_key_state);
        return true;
    }
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::isWithinDelta(const uint16_t &value, const uint16_t &new_value, const uint16_t &min_delta)
{
    int32_t signed_value = value;
    int32_t min = static_cast<int32_t>(new_value) - min_delta;
    int32_t max = static_cast<int32_t>(new_value) + min_delta;
    return (signed_value >= min && signed_value <= max);
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::addJoystickEvent(ShieldEvent &event, KeyType key, const uint16_t &current_value, const uint16_t& new_value, const int16_t &min_delta)
{
    if(isWithinDelta(current_value, new_value, min_delta))
    {
        event.key_event[uint8FromKeyType(key)] = KeyEventType::None;
        event.key_state[uint8FromKeyType(key)] = KeyEventType::None;
        return false;
    }
    else
    {
        event.key_event[uint8FromKeyType(key)] = KeyEventType::Changed;
        event.key_state[uint8FromKeyType(key)] = KeyEventType::Changed;
        return true;
    }
}

//--------------------------------------------------------------------------------------------------

void JoystickShield::setJoystickMinDelta(uint16_t min_delta)
{
    min_potentiometer_readout_delta = min_delta;
}


} // namespace Funduino
