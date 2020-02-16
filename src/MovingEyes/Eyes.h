#pragma once

#include "EyesMechanics.h"
#include "MovingEyes.h"
#include <inttypes.h>

namespace EyeMech
{

//--------------------------------------------------------------------------------------------------

struct Position : public Range
{
    // TODO: ref
    Position(const Range &range, int8_t value);

    int8_t value;
};

struct Constraint
{
    virtual int8_t trim(const Position &upper_lid, const Position &lower_lid) const = 0;
};

//--------------------------------------------------------------------------------------------------

struct LidConstraint : public Constraint
{
    int8_t trim(const Position &upper_lid, const Position &lower_lid) const override;

    LidConstraint(int8_t min_distance);

    int8_t min_distance;
};

//--------------------------------------------------------------------------------------------------

struct Constraints
{
    struct Lid
    {
        LidConstraint lid;
    };

    Lid left;
    Lid right;
};
//--------------------------------------------------------------------------------------------------

/**
 * Implementation to interface with moving eyes with simple collision avoidance.
 */
struct Eyes : public EyesMechanics
{
    /**
     *
     * @param wire @see EyesMechanics constructor
     * @param constraints constraints to apply to the actuation values to avoid collisions
     * @param limits @see EyesMechanics constructor
     * @param compensation @see EyesMechanics constructor
     * @param evaluator @see EyesMechanics constructor
     * @param balancer @see EyesMechanics constructor
     * @param do_initial_move_zero @see EyesMechanics constructor
     */
    Eyes(TwoWire &wire,
         const Constraints &constraints,
         const Limits &limits,
         const CompensationAngles &compensation,
         PCA9685_ServoEvaluator &evaluator,
         PCA9685_PhaseBalancer balancer,
         bool do_initial_move_zero);

    ~Eyes() override = default;

    void setActuation(EyesActuation &actuation) override;

protected:
    Constraints constraints;
    void trimToConstraints(EyesActuation &constrained_actuation) const;
};

//--------------------------------------------------------------------------------------------------
Constraints constraintsDefault();

} // namespace EyeMech