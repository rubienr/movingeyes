#include "Eyes.h"

//--------------------------------------------------------------------------------------------------

namespace EyeMech
{

//--------------------------------------------------------------------------------------------------

Position::Position(const Range &range, int8_t value) : Range(range), value{ value }
{
    autoComplete();
}

//--------------------------------------------------------------------------------------------------

int8_t LidConstraint::trim(const Position &upper_lid, const Position &lower_lid) const
{
    // when closed, lids can not go beyond the limit
    int8_t new_lower_lid_value{ lower_lid.value };

    if((lower_lid.value + min_distance) >= upper_lid.value)
    {
        Serial.print("upper: ");
        Serial.print(upper_lid.value);
        Serial.print(" lower: ");
        Serial.print(lower_lid.value);

        new_lower_lid_value = lower_lid.value + min_distance;

        Serial.print(" => lower: ");
        Serial.println(lower_lid.value);
    }

    return new_lower_lid_value;
}

//--------------------------------------------------------------------------------------------------

LidConstraint::LidConstraint(int8_t min_distance) : min_distance(min_distance) {}

//--------------------------------------------------------------------------------------------------

Eyes::Eyes(TwoWire &wire,
           const Constraints &constraints,
           const Limits &limits,
           const CompensationAngles &compensation,
           PCA9685_ServoEvaluator &evaluator,
           PCA9685_PhaseBalancer balancer,
           bool do_initial_move_zero)
: EyesMechanics(wire, limits, compensation, evaluator, balancer, do_initial_move_zero), constraints{ constraints }
{
}

//--------------------------------------------------------------------------------------------------

void Eyes::setActuation(EyesActuation &actuation)
{
    trimToConstraints(actuation);
    EyesActuationHelper::println(actuation, "Eyes::setActuation: ");
    EyesMechanics::setActuation(actuation);
}

//--------------------------------------------------------------------------------------------------

void Eyes::trimToConstraints(EyesActuation &constrained_actuation) const
{
    // apply eyes constraints: elevation and bearing dominate above lid
    // if eyes direction is top/bottom-left/right the eye mechanics collides with the eyes lids
    // TODO: model distance from eye to lid depending on elevation/bearing

    // apply lid constraints: upper dominates above lower
    constrained_actuation.left.lid.lower =
    constraints.left.lid.trim({ mechanic_limits.left.lid.upper, constrained_actuation.left.lid.upper },
                              { mechanic_limits.left.lid.lower, constrained_actuation.left.lid.lower });

    constrained_actuation.right.lid.lower =
    constraints.right.lid.trim({ mechanic_limits.right.lid.upper, constrained_actuation.right.lid.upper },
                               { mechanic_limits.right.lid.lower, constrained_actuation.right.lid.lower });


}

//--------------------------------------------------------------------------------------------------

Constraints constraintsDefault() { return { { { 10 } }, { { 10 } } }; }
} // namespace EyeMech