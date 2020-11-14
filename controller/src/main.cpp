#include "configuration.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <MovingEyes/EyesMechanics.h>
#include <PCA9685.h>
#include <Wire.h>
#include <pinutils.h>

#include "FunduinoJoystickShield/JoystickShield.h"
#include "MovingEyes/Eyes.h"
#include "MovingEyes/MovingEyes.h"

//--------------------------------------------------------------------------------------------------

using namespace Funduino;
using namespace EyeMech;

//--------------------------------------------------------------------------------------------------

namespace
{

//--------------------------------------------------------------------------------------------------

int8_t convertToDegree(const float &normalized, const Range &range)
{
    return static_cast<int8_t>(normalized * range.range + range.min);
}

} // namespace

//--------------------------------------------------------------------------------------------------

struct Resources : public EventReceiver
{
    struct PreInit
    {
        PCA9685_ServoEvaluator servo_evaluator{ config::getConfiguredServoEvaluator() };
        Limits mechanic_limits{ config::getConfiguredLimits() };
        CompensationAngles compensation{ config::getConfiguredCompensationAngles() };
        Constraints constraints{ config::getConfiguredConstraints() };
    } pre_init;

    // TogglePin led_x{ LED_BUILTIN_TX };
    // TogglePin led_y{ LED_BUILTIN_RX };

    JoystickShield input_device{ config::getConfiguredJoystickShieldPinout() };
    Eyes eyes{ Wire,
               pre_init.constraints,
               pre_init.mechanic_limits,
               pre_init.compensation,
               pre_init.servo_evaluator,
               PCA9685_PhaseBalancer_Weaved,
               true };

    //----------------------------------------------------------------------------------------------
    void setup()
    {
#ifdef ARDUINO_AVR_LEONARDO
        Serial.begin(230400);
#elif defined(ARDUINO_AVR_NANO)
        Serial.begin(57600);
#endif
        while(!Serial)
            delay(10);
        Serial.print("\n\n\n");
        Serial.println("Resources::setup");

        input_device.setup();
        eyes.setup();
        input_device.setEventReceiver(this);
        Serial.println("Resources::setup: done");
    }

    //----------------------------------------------------------------------------------------------
    void process() { input_device.process(); }

    //----------------------------------------------------------------------------------------------

    bool take(const Funduino::ShieldEvent &e) override
    {
        static EyesActuation actuation;
        ScopedTogglePin led{ LED_BUILTIN };
        bool consumed{ false };
        ShieldEventHelper::println(e, "Resources::take: ");

        switch(e.key)
        {

        case KeyType::A:
            break;

        case KeyType::B:
            if(!consumed && e.event == KeyEventType::Pressed)
            {
                actuation.right.lid.upper = 0;
                actuation.right.lid.lower = 0;
                consumed = true;
            }
            else if(!consumed && e.event == KeyEventType::Released)
            {
                actuation.right.lid.upper = 30;
                actuation.right.lid.lower = -30;
                consumed = true;
            }
            break;

        case KeyType::C:
            break;

        case KeyType::D:
            if(!consumed && e.event == KeyEventType::Pressed)
            {
                actuation.left.lid.upper = 0;
                actuation.left.lid.lower = 0;
                consumed = true;
            }
            else if(!consumed && e.event == KeyEventType::Released)
            {
                actuation.left.lid.upper = 30;
                actuation.left.lid.lower = -30;
                consumed = true;
            }
            break;

        case KeyType::E:
            break;

        case KeyType::F:
            break;

        case KeyType::X:
        case KeyType::Y:
            if(!consumed && e.event == KeyEventType::Changed)
            {

                actuation.bearing = convertToDegree(input_device.getJoystickData().x.getNormalizedValue(),
                                                    eyes.getMechanicLimits().bearing);
                // led_x.toggle();

                actuation.elevation = convertToDegree(input_device.getJoystickData().y.getNormalizedValue(),
                                                      eyes.getMechanicLimits().elevation);
                // led_y.toggle();
                consumed = true;
            }
            break;

        case KeyType::Z:
            if(!consumed && e.event == KeyEventType::Pressed)
            {
                actuation.left.lid.upper = 0;
                actuation.left.lid.lower = 0;
                actuation.right.lid.upper = 0;
                actuation.right.lid.lower = 0;
                consumed = true;
            }
            else if(!consumed && e.event == KeyEventType::Released)
            {
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

        if(consumed)
        {
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
