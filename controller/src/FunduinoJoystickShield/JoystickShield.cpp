#include "JoystickShield.h"
#include "ShieldState.h"
#include <Arduino.h>

namespace funduino {

//--------------------------------------------------------------------------------------------------

JoystickShield::JoystickShield(const ArduinoPinConfiguration &io_pins) : pins(io_pins) {}

//--------------------------------------------------------------------------------------------------

void JoystickShield::setup() {
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

    state.joystick.x.update(static_cast<uint16_t>(analogRead(pins.input.joystick.x)));
    state.joystick.y.update(static_cast<uint16_t>(analogRead(pins.input.joystick.y)));
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::process() {

    ShieldState &new_state = readState();

    // on key event
    bool has_changed{ state.keys != new_state.keys };

    // on fuzzy x/y change
    if(!isWithinDelta(new_state.joystick.x.value, state.joystick.x.value, min_potentiometer_readout_delta) ||
       !isWithinDelta(new_state.joystick.y.value, state.joystick.y.value, min_potentiometer_readout_delta)) {
        has_changed = true;
    }

    // on z-key event
    if(state.joystick.z != new_state.joystick.z) {
        has_changed = true;
    }

    // update state and trigger events
    if(has_changed) {
        // ShieldStateHelper::println(state, "JoystickShield::process: old state ");
        // ShieldStateHelper::println(new_state, "JoystickShield::process: new state ");
        if(event_receiver != nullptr) {
            static ShieldEvent event;
            updateEvent(event, new_state);
            // ShieldEventHelper::println(event);
            state = new_state;

            if(isCalibrated()) {
                event_receiver->take(event);
            }
        } else {
            state = new_state;
        }
    }

    return has_changed;
}

//--------------------------------------------------------------------------------------------------

ShieldState &JoystickShield::readState() {
    static ShieldState current_state;

    current_state.keys.a = keyStateFromUint8(static_cast<uint8_t>(digitalRead(pins.input.keys.a)));
    current_state.keys.b = keyStateFromUint8(static_cast<uint8_t>(digitalRead(pins.input.keys.b)));
    current_state.keys.c = keyStateFromUint8(static_cast<uint8_t>(digitalRead(pins.input.keys.c)));
    current_state.keys.d = keyStateFromUint8(static_cast<uint8_t>(digitalRead(pins.input.keys.d)));
    current_state.keys.e = keyStateFromUint8(static_cast<uint8_t>(digitalRead(pins.input.keys.e)));
    current_state.keys.f = keyStateFromUint8(static_cast<uint8_t>(digitalRead(pins.input.keys.f)));

    current_state.joystick.x.update(static_cast<uint16_t>(analogRead(pins.input.joystick.x)));
    current_state.joystick.y.update(static_cast<uint16_t>(analogRead(pins.input.joystick.y)));
    current_state.joystick.z =
    keyStateFromUint8(static_cast<uint8_t>(digitalRead(pins.input.joystick.z)));

    // TODO: calibrate x/y center position

    return current_state;
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::setEventReceiver(EventReceiver *receiver) {
    if(event_receiver != receiver) {
        event_receiver = receiver;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------

void JoystickShield::updateEvent(ShieldEvent &event, const ShieldState &new_state) const {
    event.event = KeyEventType::None;
    event.key = KeyType::Void;

    // try detect state change event
    updateIfKeyEvent(event, KeyType::A, state.keys.a, new_state.keys.a);
    updateIfKeyEvent(event, KeyType::B, state.keys.b, new_state.keys.b);
    updateIfKeyEvent(event, KeyType::C, state.keys.c, new_state.keys.c);
    updateIfKeyEvent(event, KeyType::D, state.keys.d, new_state.keys.d);
    updateIfKeyEvent(event, KeyType::E, state.keys.e, new_state.keys.e);
    updateIfKeyEvent(event, KeyType::F, state.keys.f, new_state.keys.f);

    updateIfJoystickEvent(event, KeyType::X, state.joystick.x.value, new_state.joystick.x.value,
                          min_potentiometer_readout_delta);
    updateIfJoystickEvent(event, KeyType::Y, state.joystick.y.value, new_state.joystick.y.value,
                          min_potentiometer_readout_delta);
    updateIfKeyEvent(event, KeyType::Z, state.joystick.z, new_state.joystick.z);

    // update complete shield state
    event.shield_state = new_state;
    event.shield_state.joystick.x.copyFrom(new_state.joystick.x);
    event.shield_state.joystick.y.copyFrom(new_state.joystick.y);
}

//--------------------------------------------------------------------------------------------------

KeyEventType JoystickShield::eventTypeFromKeyState(KeyStateType ks) {
    if(ks == KeyStateType::Pressed) {
        return KeyEventType::Pressed;
    }
    if(ks == KeyStateType::Released) {
        return KeyEventType::Released;
    }
    if(ks == KeyStateType::Undefined) {
        return KeyEventType::None;
    }
    return KeyEventType::None;
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::updateIfKeyEvent(ShieldEvent &shield_event,
                                      const KeyType key,
                                      const KeyStateType current_key_state,
                                      const KeyStateType new_key_state) {
    if(current_key_state == new_key_state) {
        return false;
    }

    shield_event.key = key;
    shield_event.event = eventTypeFromKeyState(new_key_state);

    return true;
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::isWithinDelta(const uint16_t &value, const uint16_t &new_value, const uint16_t &min_delta) {
    int32_t signed_value = value;
    int32_t min = static_cast<int32_t>(new_value) - min_delta;
    int32_t max = static_cast<int32_t>(new_value) + min_delta;
    return (signed_value >= min && signed_value <= max);
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::updateIfJoystickEvent(ShieldEvent &event,
                                           KeyType key,
                                           const uint16_t &current_value,
                                           const uint16_t &new_value,
                                           const uint16_t &min_delta) {
    if(isWithinDelta(current_value, new_value, min_delta)) {
        return false;
    }

    event.key = key;
    event.event = KeyEventType::Changed;

    return true;
}

//--------------------------------------------------------------------------------------------------

void JoystickShield::setJoystickMinDelta(uint16_t min_delta) {
    min_potentiometer_readout_delta = min_delta;
}

//--------------------------------------------------------------------------------------------------

bool JoystickShield::isCalibrated() {
    static bool calibration_finished{ false };
    if(calibration_finished) {
        return true;
    }

    constexpr const uint16_t max_expected_value{ 1023 };
    constexpr const uint16_t mandatory_min_seen_max_potentiometer_value{ static_cast<uint16_t>(0.9 * max_expected_value) };
    constexpr const uint16_t mandatory_max_seen_min_potentiometer_value{ static_cast<uint16_t>(0.1 * max_expected_value) };

    static bool is_calibrated{ false };

    auto verify_calibration = [&](const Potentiometer &p) {
        if(p.min > mandatory_max_seen_min_potentiometer_value) {
            return false;
        }
        if(p.max < mandatory_min_seen_max_potentiometer_value) {
            return false;
        }
        return true;
    };

    auto is_home_position = [](const Potentiometer &p) {
        const uint16_t min = p.center - 20, max = p.center + 20;
        return min <= p.value && p.value <= max;
    };

    is_calibrated = verify_calibration(state.joystick.x) && verify_calibration(state.joystick.y);

    calibration_finished =
    is_calibrated && is_home_position(state.joystick.x) && is_home_position(state.joystick.y);

    return calibration_finished;
}

} // namespace funduino
