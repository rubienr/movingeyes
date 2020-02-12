
#include "MovingEyes.h"
#include "EyesMechanics.h"

namespace EyeMech
{

//--------------------------------------------------------------------------------------------------

MovingEyes::MovingEyes(const Limits &limits, PCA9685_ServoEvaluator &evaluator, bool do_initial_move_zero)
: EyesMechanics(evaluator), mechanic_limits(limits), do_initial_move_zero(do_initial_move_zero)
{
}

//--------------------------------------------------------------------------------------------------

void MovingEyes::setActuation(const EyesActuation &eyes_actuation)
{
    // EyesActuationHelper::println(eyes_actuation, "MovingEyes::setActuation: ");
    *static_cast<EyesActuation *>(&raw_actuation_values) = eyes_actuation;
    trimToMechanicalLimits();
}

//--------------------------------------------------------------------------------------------------

void MovingEyes::trimToMechanicalLimits()
{
    trim(raw_actuation_values.left.lid.upper, mechanic_limits.left.lid.upper);
    trim(raw_actuation_values.left.lid.lower, mechanic_limits.left.lid.upper);

    trim(raw_actuation_values.right.lid.upper, mechanic_limits.left.lid.upper);
    trim(raw_actuation_values.right.lid.lower, mechanic_limits.left.lid.upper);

    trim(raw_actuation_values.elevation, mechanic_limits.elevation);
    trim(raw_actuation_values.bearing, mechanic_limits.bearing);
}

//--------------------------------------------------------------------------------------------------

void MovingEyes::setup()
{
    EyesMechanics::setup();

    if(do_initial_move_zero) setActuation({});
    process();
}

//--------------------------------------------------------------------------------------------------

Limits mechanicLimitsDefault()
{
    Limits l;

    l.bearing = { -90, 90 };
    l.elevation = { -90, 90 };
    l.left.lid.upper = { -50, 50 };
    l.left.lid.lower = { -50, 50 };
    l.left.lid.upper = { -50, 50 };
    l.left.lid.lower = { -50, 50 };

    l.autoComplete();
    return l;
}

//--------------------------------------------------------------------------------------------------

PCA9685_ServoEvaluator servoEvaluatorMg90sMicroservo()
{
    constexpr uint16_t min = 102, max = 512,
                       center = static_cast<uint16_t>((static_cast<float>(max) - min) / 2 + min);

    constexpr int16_t center_bias = +5;
    constexpr int16_t min_bias = -7;
    constexpr int16_t max_bias = +18;

    return { min + min_bias, center + center_bias, max + max_bias };
}

//--------------------------------------------------------------------------------------------------

PCA9685_ServoEvaluator servoEvaluatorDefault() { return {}; }

} // namespace EyeMech
