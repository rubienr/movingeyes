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
    TogglePin led_x{ 0 };
    TogglePin led_y{ 1 };
    TogglePin led{ LED_BUILTIN };

    JoystickShield input_device;
    MovingEyes eyes;

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

        // Serial.println("Resources::take");
        // ShieldEventHelper::print(e);
        bool consumed{ false };
        if(e.key_event[uint8FromKeyType(KeyType::X)] == KeyEventType::Changed)
        {

            actuation.bearing = input_device.getJoystickData().x.getNormalizedValue() * 180 - 90;
            led_x.toggle();
            consumed = true;
        }

        if(e.key_event[uint8FromKeyType(KeyType::Y)] == KeyEventType::Changed)
        {
            actuation.elevation = input_device.getJoystickData().y.getNormalizedValue() * 180 - 90;
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
