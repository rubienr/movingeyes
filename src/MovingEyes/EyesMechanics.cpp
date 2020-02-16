
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

//--------------------------------------------------------------------------------------------------

Limits mechanicLimitsDefault()
{
    Limits l;

    l.bearing = { -50, 50 };
    l.elevation = { -50, 50 };
    l.left.lid.upper = { -50, 50 };
    l.left.lid.lower = { -50, 50 };
    l.right.lid.upper = { -50, 50 };
    l.right.lid.lower = { -50, 50 };

    l.autoComplete();
    return l;
}

//--------------------------------------------------------------------------------------------------

Limits mechanicLimitsMovingEyes()
{
    // manually measured min/max mechanical values without considering collisions.
    struct Constraints
    {

        struct
        {
            int8_t bearing = -55;
            int8_t elevation = -55;
            int8_t lower_lid = -63;
            int8_t upper_lid = -80;
        } min;

        struct
        {
            int8_t bearing = 55;
            int8_t elevation = 50;
            int8_t lower_lid = 75;
            int8_t upper_lid = 63;
        } max;
    } physical_limits;

    Limits l;
    l.bearing = { physical_limits.min.bearing, physical_limits.max.bearing };
    l.elevation = { physical_limits.min.elevation, physical_limits.max.elevation };
    l.left.lid.upper = { physical_limits.min.upper_lid, physical_limits.max.upper_lid };
    l.left.lid.lower = { physical_limits.min.lower_lid, physical_limits.max.lower_lid };
    l.right.lid.upper = { physical_limits.min.upper_lid, physical_limits.max.upper_lid };
    l.right.lid.lower = { physical_limits.min.lower_lid, physical_limits.max.lower_lid };
    l.autoComplete();
    return l;
}


//--------------------------------------------------------------------------------------------------

CompensationAngles compensationAnglesDefault() { return { { 0, 0 }, { 0, 0 }, 0, 0 }; }

//--------------------------------------------------------------------------------------------------

CompensationAngles compensationAnglesMovingEyes()
{
    CompensationAngles c;

    // TODO
    //c.left = { -40, +35 };
    //c.right = { -36, +30 };

    c.left = { -35, +30 };
    c.right = { -31, +30 };

    c.elevation = -10;
    c.bearing = -4;

    return c;
}

} // namespace EyeMech
