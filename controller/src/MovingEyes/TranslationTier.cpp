
#include "TranslationTier.h"
#include "ServoTier.h"

namespace eyes {
namespace translation {

//--------------------------------------------------------------------------------------------------

TranslationTier::TranslationTier(TwoWire &wire,
                                 const Limits &limits,
                                 const CompensationAngles &compensation,
                                 PCA9685_ServoEvaluator &evaluator,
                                 PCA9685_PhaseBalancer balancer,
                                 bool do_initial_move_zero)
: ServoTier(wire, evaluator, balancer), mechanic_limits{ limits }, do_initial_move_zero{ do_initial_move_zero } {
    raw_actuation_values.elevation.bias = compensation.elevation;
    raw_actuation_values.bearing.bias = compensation.bearing;

    raw_actuation_values.left.lid.upper.bias = compensation.left.lid.upper;
    raw_actuation_values.left.lid.lower.bias = compensation.left.lid.lower;

    raw_actuation_values.right.lid.upper.bias = compensation.right.lid.upper;
    raw_actuation_values.right.lid.lower.bias = compensation.right.lid.lower;
}

//--------------------------------------------------------------------------------------------------

void TranslationTier::setActuation(servo::EyesActuation &eyes_actuation) {
    trimToMechanicalLimits(eyes_actuation);
    *static_cast<servo::BiasedEyesActuation *>(&raw_actuation_values) =
    toBiasedEyesActuation(eyes_actuation, raw_actuation_values);
    // intern::RawActuationHelper::println(raw_actuation_values, "EyesMechanics::setActuation: ");
}

//--------------------------------------------------------------------------------------------------

void TranslationTier::trimToMechanicalLimits(servo::EyesActuation &actuation) {
    trim(actuation.left.lid.upper, mechanic_limits.left.lid.upper);
    trim(actuation.left.lid.lower, mechanic_limits.left.lid.upper);

    trim(actuation.right.lid.upper, mechanic_limits.left.lid.upper);
    trim(actuation.right.lid.lower, mechanic_limits.left.lid.upper);

    trim(actuation.elevation, mechanic_limits.elevation);
    trim(actuation.bearing, mechanic_limits.bearing);
}

//--------------------------------------------------------------------------------------------------

void TranslationTier::setup() {
    ServoTier::setup();

    if(do_initial_move_zero) {
        servo::EyesActuation a;
        setActuation(a);
    }
    process();
}

//--------------------------------------------------------------------------------------------------

const Limits &TranslationTier::getMechanicLimits() { return mechanic_limits; }

//--------------------------------------------------------------------------------------------------

servo::BiasedEyesActuation &
TranslationTier::toBiasedEyesActuation(const servo::EyesActuation &actuation_values,
                                       const servo::intern::RawActuation &bias_values) {
    static servo::BiasedEyesActuation converted;

    // update bias values
    converted = *static_cast<const servo::BiasedEyesActuation *>(&bias_values);

    // update actuation values
    converted.elevation.value = actuation_values.elevation;
    converted.bearing.value = actuation_values.bearing;

    converted.left.lid.upper.value = actuation_values.left.lid.upper;
    converted.left.lid.lower.value = actuation_values.left.lid.lower;

    converted.right.lid.upper.value = actuation_values.right.lid.upper;
    converted.right.lid.lower.value = actuation_values.right.lid.lower;


    return converted;
}


} // namespace translation
} // namespace eyes