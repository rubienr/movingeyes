#include "configuration.h"

#include <Arduino.h>
#include <MovingEyes/TranslationTier.h>
#include <PCA9685.h>
#include <Wire.h>
#include <elapsedMillis.h>
#include <pinutils.h>

#include "FunduinoJoystickShield/JoystickShield.h"
#include "MovingEyes/MovingEyes.h"
#include "MovingEyes/ServoTier.h"


//--------------------------------------------------------------------------------------------------

namespace {

//--------------------------------------------------------------------------------------------------

int8_t convertToDegree(const float &normalized, const eyes::translation::Range &range) {
    return static_cast<int8_t>(normalized * range.range + range.min);
}

} // namespace

//--------------------------------------------------------------------------------------------------

struct Resources : public funduino::EventReceiver {
    struct PreInit {
        PCA9685_ServoEvaluator servo_evaluator{ config::getConfiguredServoEvaluator() };
        eyes::translation::Limits mechanic_limits{ config::getConfiguredLimits() };
        eyes::translation::CompensationAngles compensation{ config::getConfiguredCompensationAngles() };
        eyes::Constraints constraints{ config::getConfiguredConstraints() };
    } pre_init;

    // TogglePin led_x{ LED_BUILTIN_TX };
    // TogglePin led_y{ LED_BUILTIN_RX };

    funduino::JoystickShield input_device{ config::getConfiguredJoystickShieldPinout() };
    eyes::MovingEyes eyes{ Wire,
                           pre_init.constraints,
                           pre_init.mechanic_limits,
                           pre_init.compensation,
                           pre_init.servo_evaluator,
                           PCA9685_PhaseBalancer_Weaved,
                           true };


#if defined(SERVO_IDLE_TIMEOUT_MS)
    struct {
        elapsedMillis elapsed_millis{ 0 };
        bool is_idle{ false };
    } timeout;
#endif

    //----------------------------------------------------------------------------------------------

    void setup() {
        Serial.begin(SERIAL_BAUD_RATE);
        while(!Serial)
            delay(10);
        Serial.print("\n\n\n");
        Serial.println("Resources::setup");

        input_device.setup();
        eyes.setup();
        input_device.setEventReceiver(this);

#if defined(SERVO_IDLE_TIMEOUT_MS)
        Serial.print("Resources::setup: servo timeout (");
        Serial.print(SERVO_IDLE_TIMEOUT_MS);
        Serial.println("ms) is enabled; they will most likely jerk when en-/disabling");
#endif

        Serial.println("Resources::setup: done");

        Serial.println("\nResources::setup: Joystick Calibration:");
        Serial.println(
        "Resources::setup:   1. Move the joystick in all axis' min/max position (turn a circle).");
        Serial.println(
        "Resources::setup:   2. Leave the joystick alone to move to its idle position.");
        Serial.println("Resources::setup:   3. Have fun!\n");

#if defined(SERVO_IDLE_TIMEOUT_MS)
        timeout.elapsed_millis = 0;
#endif
    }

    //----------------------------------------------------------------------------------------------
    void process() {
        input_device.process();
#if defined(SERVO_IDLE_TIMEOUT_MS)
        if(timeout.elapsed_millis > SERVO_IDLE_TIMEOUT_MS) {

            if(!timeout.is_idle) {
#if defined(DEBUG_MAIN)
                Serial.println("Resources::process: disable servos");
#endif
                eyes.disableServos();
                timeout.is_idle = true;
            }
        } else {
            if(timeout.is_idle) {
#if defined(DEBUG_MAIN)
                Serial.println("Resources::process: enable servos");
#endif
                eyes.enableServos();
                timeout.is_idle = false;
            }
        }
#endif
    }

    //----------------------------------------------------------------------------------------------

    bool take(const funduino::ShieldEvent &e) override {
        static eyes::servo::EyesActuation actuation;
        ScopedTogglePin led{ LED_BUILTIN };
        bool consumed{ false };
#if defined(DEBUG_MAIN)
        funduino::ShieldEventHelper::println(e, "Resources::take: ");
#endif

        switch(e.key) {

        case funduino::KeyType::A:
            break;

        case funduino::KeyType::B:
            if(!consumed && e.event == funduino::KeyEventType::Pressed) {
                actuation.right.lid.upper = 0;
                actuation.right.lid.lower = 0;
                consumed = true;
            } else if(!consumed && e.event == funduino::KeyEventType::Released) {
                actuation.right.lid.upper = 30;
                actuation.right.lid.lower = -30;
                consumed = true;
            }
            break;

        case funduino::KeyType::C:
            break;

        case funduino::KeyType::D:
            if(!consumed && e.event == funduino::KeyEventType::Pressed) {
                actuation.left.lid.upper = 0;
                actuation.left.lid.lower = 0;
                consumed = true;
            } else if(!consumed && e.event == funduino::KeyEventType::Released) {
                actuation.left.lid.upper = 30;
                actuation.left.lid.lower = -30;
                consumed = true;
            }
            break;

        case funduino::KeyType::E:
            break;

        case funduino::KeyType::F:
            break;

        case funduino::KeyType::X:
        case funduino::KeyType::Y:
            if(!consumed && e.event == funduino::KeyEventType::Changed) {

                actuation.bearing = convertToDegree(input_device.getJoystickData().x.getNormalizedValue(),
                                                    eyes.getMechanicLimits().bearing);
                // led_x.toggle();

                actuation.elevation = convertToDegree(input_device.getJoystickData().y.getNormalizedValue(),
                                                      eyes.getMechanicLimits().elevation);
                // led_y.toggle();
                consumed = true;
            }
            break;

        case funduino::KeyType::Z:
            if(!consumed && e.event == funduino::KeyEventType::Pressed) {
                actuation.left.lid.upper = 0;
                actuation.left.lid.lower = 0;
                actuation.right.lid.upper = 0;
                actuation.right.lid.lower = 0;
                consumed = true;
            } else if(!consumed && e.event == funduino::KeyEventType::Released) {
                actuation.left.lid.upper = 50;
                actuation.left.lid.lower = -50;
                actuation.right.lid.upper = 50;
                actuation.right.lid.lower = -50;
                consumed = true;
            }
            break;

        default:
            break;
        }

        if(consumed) {
#if defined(SERVO_IDLE_TIMEOUT_MS)
            timeout.elapsed_millis = 0;
#endif
            eyes.setActuation(actuation);
            eyes.process();
        }

        return consumed;
    }

} r;

//--------------------------------------------------------------------------------------------------

void setup() { r.setup(); }

//--------------------------------------------------------------------------------------------------

void loop() { r.process(); }
