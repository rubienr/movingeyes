#pragma once

#include "EventRecever.h"
#include "ShieldState.h"
#include <pins_arduino.h>

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

struct ArduinoPinConfiguration
{
    struct Input
    {
        struct Keys
        {
            const uint8_t a, b, c, d, e, f; // digital, buttons
        } keys;

        struct Joystick
        {
            const uint8_t x, y; // analogue, potentiometer
            const uint8_t z;    // digital, button
        } joystick;

    } input;
};

//--------------------------------------------------------------------------------------------------

constexpr ArduinoPinConfiguration leonardoPinout()
{
    return { { { 2, 3, 4, 5, 6, 7 }, { A0, A1, 8 } } };
}

//--------------------------------------------------------------------------------------------------

/**
 * Pinout with A/B moved to D9/D10 to leave SDA/SCL untouched.
 * Leonardo compatible pinout with A/B moved to D9/D10 to leave D2/D3 free for SDA/SCL.
 * Needs removal of the respective pins (interrupt, unsolder) and reconnecting to D9/D10
 *
 */
constexpr ArduinoPinConfiguration leonardoPinoutSerial()
{
    return { { { 9, 10, 4, 5, 6, 7 }, { A0, A1, 8 } } };
}

//--------------------------------------------------------------------------------------------------

class JoystickShield
{
public:
    explicit JoystickShield(const ArduinoPinConfiguration &io_pins = leonardoPinout());

    /**
     * Replaced the old receiver (if any) wit the new receiver.
     * @param receiver new receiver or nullptr
     * @return true if old receiver != new receiver, false otherwise
     */
    bool setEventReceiver(EventReceiver *receiver);

    void setup();

    /**
     * Reads from periphery and sends event on key state changed or potentiometer value changed.
     * @return true if state change is detected
     */
    bool process();

    /**
     * @return the current joystick and Z-key readout
     */
    const ShieldState::Joystick &getJoystickData() { return state.joystick; }

    /**
     * @return the current keys (A,B,C,D,E,F) readout
     */
    const ShieldState::KeysState &getKeysData() { return state.keys; }

    /**
     * Joystick potentiometer changes below won't be detected as change to mitigate noise/jitter.
     * @param min_delta good values are between 1 and 7
     */
    void setJoystickMinDelta(uint16_t min_delta);

protected:
    uint16_t min_potentiometer_readout_delta{ 2 };

    ArduinoPinConfiguration pins;

    ShieldState &readState();
    ShieldState state /*{ { KeyStateType::Released, KeyStateType::Released, KeyStateType::Released,
                          KeyStateType::Released, KeyStateType::Released, KeyStateType::Released },
                        { KeyStateType::Released } }*/
    ;
    EventReceiver *event_receiver{ nullptr };

    void updateEvent(ShieldEvent &event, const ShieldState &new_state) const;
    static KeyEventType eventTypeFromKeyState(KeyStateType ks);
    static bool updateIfKeyEvent(ShieldEvent &shield_event,
                                 const KeyType key,
                                 const KeyStateType current_key_state,
                                 const KeyStateType new_key_state);

    static bool isWithinDelta(const uint16_t &value, const uint16_t &new_value, const uint16_t &min_delta);
    static bool updateIfJoystickEvent(ShieldEvent &event,
                                      KeyType key,
                                      const uint16_t &current_value,
                                      const uint16_t &new_value,
                                      const uint16_t &min_delta);

    bool isCalibrated();

private:
};

} // namespace Funduino
