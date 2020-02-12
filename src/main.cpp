#include "FunduinoJoystickShield/JoystickShield.h"
#include "MovingEyes/EyesMechanics.h"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <MovingEyes/MovingEyes.h>
#include <pinutils.h>

using namespace Funduino;
using namespace EyeMech;

//--------------------------------------------------------------------------------------------------

struct Resources : public EventReceiver
{
    struct EarlyInitializer
    {
        PCA9685_ServoEvaluator servo_evaluator{ servoEvaluatorMg90sMicroservo() };
    } early_init;

    TogglePin led_x{ 0 };
    TogglePin led_y{ 1 };
    TogglePin led{ LED_BUILTIN };

    JoystickShield input_device;
    MovingEyes eyes{ mechanicLimitsDefault(), early_init.servo_evaluator };

    //----------------------------------------------------------------------------------------------
    void setup()
    {

        Serial.begin(230400);
        while(!Serial)
            delay(10);
        Serial.print("\n\n\n");
        Serial.println("Resources::setup");

        input_device.setup();
        eyes.setup();
        input_device.setEventReceiver(this);
    }

    //----------------------------------------------------------------------------------------------
    void process() { input_device.process(); }

    //----------------------------------------------------------------------------------------------

    bool take(const Funduino::ShieldEvent &e) override
    {
        static EyesActuation actuation;
        led.toggle();
        // ShieldEventHelper::println(e, "Resources::take: ");

        bool consumed{ false };
        if(e.key == KeyType::X && e.event == KeyEventType::Changed)
        {

            actuation.bearing =
            static_cast<int8_t>(input_device.getJoystickData().x.getNormalizedValue() * 180 - 90);
            led_x.toggle();
            consumed = true;
        }

        if(e.key == KeyType::Y && e.event == KeyEventType::Changed)
        {
            actuation.elevation =
            static_cast<int8_t>(input_device.getJoystickData().y.getNormalizedValue() * 180 - 90);
            led_y.toggle();
            consumed = true;
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
