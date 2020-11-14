#pragma once

#include "ServoTier.h"
#include "TranslationTier.h"
#include <inttypes.h>

namespace eyes {

//--------------------------------------------------------------------------------------------------

struct Position : public translation::Range {
    // TODO: ref
    Position(const Range &range, int8_t value);

    int8_t value;
};

struct Constraint {
    virtual int8_t trim(const Position &upper_lid, const Position &lower_lid) const = 0;
};

//--------------------------------------------------------------------------------------------------

struct LidConstraint : public Constraint {
    LidConstraint(int8_t min_distance);

    int8_t trim(const Position &upper_lid, const Position &lower_lid) const override;

    int8_t min_distance;
};

//--------------------------------------------------------------------------------------------------

struct Constraints {
    struct Lid {
        LidConstraint lid;
    };

    Lid left;
    Lid right;
};
//--------------------------------------------------------------------------------------------------

/**
 * Implementation to interface with moving eyes with simple collision avoidance.
 */
struct MovingEyes : public translation::TranslationTier {
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
    MovingEyes(TwoWire &wire,
               const Constraints &constraints,
               const translation::Limits &limits,
               const translation::CompensationAngles &compensation,
               PCA9685_ServoEvaluator &evaluator,
               PCA9685_PhaseBalancer balancer,
               bool do_initial_move_zero);

    ~MovingEyes() override = default;

    void setActuation(servo::EyesActuation &actuation) override;

protected:
    Constraints constraints;
    void trimToConstraints(servo::EyesActuation &constrained_actuation) const;
};

} // namespace eyes