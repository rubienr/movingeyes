
#include "EyesMechanics.h"
#include "MovingEyes.h"

namespace EyeMech
{

//--------------------------------------------------------------------------------------------------

EyesMechanics::EyesMechanics(TwoWire &wire,
                             const Limits &limits,
                             const CompensationAngles &compensation,
                             PCA9685_ServoEvaluator &evaluator,
                             PCA9685_PhaseBalancer balancer,
                             bool do_initial_move_zero)
: MovingEyes(wire, evaluator, balancer), mechanic_limits{ limits }, do_initial_move_zero{ do_initial_move_zero }
{
    raw_actuation_values.elevation.bias = compensation.elevation;
    raw_actuation_values.bearing.bias = compensation.bearing;

    raw_actuation_values.left.lid.upper.bias = compensation.left.lid.upper;
    raw_actuation_values.left.lid.lower.bias = compensation.left.lid.lower;

    raw_actuation_values.right.lid.upper.bias = compensation.right.lid.upper;
    raw_actuation_values.right.lid.lower.bias = compensation.right.lid.lower;
}

//--------------------------------------------------------------------------------------------------

void EyesMechanics::setActuation(EyesActuation &eyes_actuation)
{
    trimToMechanicalLimits(eyes_actuation);
    *static_cast<BiasedEyesActuation *>(&raw_actuation_values) =
    toBiasedEyesActuation(eyes_actuation, raw_actuation_values);
    // intern::RawActuationHelper::println(raw_actuation_values, "EyesMechanics::setActuation: ");
}

//--------------------------------------------------------------------------------------------------

void EyesMechanics::trimToMechanicalLimits(EyesActuation &actuation)
{
    trim(actuation.left.lid.upper, mechanic_limits.left.lid.upper);
    trim(actuation.left.lid.lower, mechanic_limits.left.lid.upper);

    trim(actuation.right.lid.upper, mechanic_limits.left.lid.upper);
    trim(actuation.right.lid.lower, mechanic_limits.left.lid.upper);

    trim(actuation.elevation, mechanic_limits.elevation);
    trim(actuation.bearing, mechanic_limits.bearing);
}

//--------------------------------------------------------------------------------------------------

void EyesMechanics::setup()
{
    MovingEyes::setup();

    if(do_initial_move_zero)
    {
        EyesActuation a;
        setActuation(a);
    }
    process();
}

//--------------------------------------------------------------------------------------------------

const Limits &EyesMechanics::getMechanicLimits() { return mechanic_limits; }

//--------------------------------------------------------------------------------------------------

BiasedEyesActuation &EyesMechanics::toBiasedEyesActuation(const EyesActuation &actuation_values,
                                                          const intern::RawActuation &bias_values)
{
    static BiasedEyesActuation converted;

    // update bias values
    converted = *static_cast<const BiasedEyesActuation *>(&bias_values);

    // update actuation values
    converted.elevation.value = actuation_values.elevation;
    converted.bearing.value = actuation_values.bearing;

    converted.left.lid.upper.value = actuation_values.left.lid.upper;
    converted.left.lid.lower.value = actuation_values.left.lid.lower;

    converted.right.lid.upper.value = actuation_values.right.lid.upper;
    converted.right.lid.lower.value = actuation_values.right.lid.lower;


    return converted;
}



} // namespace EyeMech
