#pragma once

#include "MovingEyes.h"

#include <HardwareSerial.h>

namespace EyeMech
{


//--------------------------------------------------------------------------------------------------

struct Range
{
    int8_t min, max; // mandatory
    uint8_t range;   // can be computed with #autoComplete()

    void autoComplete()
    {
        if(max < min)
        {
            int8_t t{ min };
            min = max;
            max = t;
        }

        range = static_cast<uint8_t>(max - min);
    }
};

//--------------------------------------------------------------------------------------------------

/**
 * The min and max limits (in degrees) the respective servo should move beyond.
 */
struct Limits
{
    struct
    {
        struct Lid
        {
            Range upper;
            Range lower;

            void autoComplete()
            {
                upper.autoComplete();
                lower.autoComplete();
            }

        } lid;

        void autoComplete() { lid.autoComplete(); }
    } left, right;

    Range elevation;
    Range bearing;

    void autoComplete()
    {
        left.autoComplete();
        right.autoComplete();
        elevation.autoComplete();
        bearing.autoComplete();
    }
};
//--------------------------------------------------------------------------------------------------

struct CompensationAngles
{
    struct
    {
        struct Lid
        {
            int8_t upper;
            int8_t lower;

        } lid;

    } left, right;

    int8_t elevation;
    int8_t bearing;
};

//--------------------------------------------------------------------------------------------------

/**
 * Implementation to interface with the eye mechanics and respecting min/max servo limits.
 * Takes actuation values and projects them to the mechanics but will trim extreme actuation beyond
 * servo limits. Caution: This implementation is not aware of mechanical collisions (for example:
 * upper and lower eye lids).
 */
struct EyesMechanics : public MovingEyes
{
    /**
     *
     * @param wire @see MovingEyes constructor
     * @param limits min and max servo limits the movement cannot go beyond
     * @param compensation to bias servos for example adjusting the upper and lower eye lids zero position
     * @param evaluator @see MovingEyes constructor
     * @param balancer @see MovingEyes constructor
     * @param do_initial_move_zero  @see MovingEyes constructor
     */
    EyesMechanics(TwoWire &wire,
                  const Limits &limits,
                  const CompensationAngles &compensation,
                  PCA9685_ServoEvaluator &evaluator,
                  PCA9685_PhaseBalancer balancer,
                  bool do_initial_move_zero);

    ~EyesMechanics() override = default;

    void setup() override;

    virtual void setActuation(EyesActuation &actuation);

    const Limits &getMechanicLimits();

protected:
    Limits mechanic_limits;
    const bool do_initial_move_zero;


    void trimToMechanicalLimits(EyesActuation &actuation);

    template <typename type_t> void trim(type_t &value, const Range &range);
    BiasedEyesActuation &toBiasedEyesActuation(const EyesActuation &actuation_values,
                                               const intern::RawActuation &bias_values);
};

//--------------------------------------------------------------------------------------------------

template <typename type_t> void EyesMechanics::trim(type_t &value, const Range &range)
{
    if(value < range.min)
    {
        value = range.min;
    }
    else if(value > range.max)
    {
        value = range.max;
    }
}

} // namespace EyeMech
