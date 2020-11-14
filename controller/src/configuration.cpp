#include "configuration.h"

#include <PCA9685.h>

#include "FunduinoJoystickShield/JoystickShield.h"
#include "MovingEyes/Eyes.h"
#include "MovingEyes/EyesMechanics.h"

//--------------------------------------------------------------------------------------------------

namespace config
{

//--------------------------------------------------------------------------------------------------

PCA9685_ServoEvaluator getConfiguredServoEvaluator() { return SERVO_EVALUATOR_CONFIG; }

//--------------------------------------------------------------------------------------------------

EyeMech::Limits getConfiguredLimits()
{
    // manually measured min/max mechanical values without considering collisions.
    struct Constraints
    {
        struct
        {
            struct
            {
                struct
                {
                    struct
                    {
                        int8_t min, max;
                    };
                } upper, lower;
            } left, right;
        };

        struct
        {
            int8_t min, max;
        } bearing;
        struct
        {
            int8_t min, max;
        } elevation;
    } physical_limits{ PHYSICAL_LIMITS_DEGREE };

    EyeMech::Limits l;

    l.bearing = { physical_limits.bearing.min, physical_limits.bearing.max };
    l.elevation = { physical_limits.elevation.min, physical_limits.elevation.max };
    l.left.lid.upper = { physical_limits.left.upper.min, physical_limits.left.upper.max };
    l.left.lid.lower = { physical_limits.left.lower.min, physical_limits.left.lower.max };
    l.right.lid.upper = { physical_limits.right.upper.min, physical_limits.right.upper.max };
    l.right.lid.lower = { physical_limits.right.lower.min, physical_limits.right.lower.max };
    l.autoComplete();
    return l;
}

//--------------------------------------------------------------------------------------------------

EyeMech::CompensationAngles getConfiguredCompensationAngles()
{
    // EyeMech::CompensationAngles c;

    // TODO
    // c.left = { -40, +35 };
    // c.right = { -36, +30 };

    return { .left = { -35, +30 }, .right = { -31, +30 }, .elevation = -10, .bearing = -4 };

    /*
    c.left = { -35, +30 };
    c.right = { -31, +30 };

    c.elevation = -10;
    c.bearing = -4;

    return c;
     */
}


//--------------------------------------------------------------------------------------------------

EyeMech::Constraints getConfiguredConstraints() { return { { { 10 } }, { { 10 } } }; }


//--------------------------------------------------------------------------------------------------

Funduino::ArduinoPinConfiguration getConfiguredJoystickShieldPinout()
{
    return JOYSTICK_SHIELD_PINS;
}


} // namespace config