#pragma once

#include "EyesMechanics.h"

#include <HardwareSerial.h>

namespace EyeMech
{

//--------------------------------------------------------------------------------------------------

struct Range
{
    int8_t min, max; // mandatory
    uint8_t range;   // can be computed with #autoComplete()

    void autoComplete() { range = abs(max(min, max) - min(min, max)); }
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
        elevation.autoComplete();
        bearing.autoComplete();
        left.autoComplete();
        right.autoComplete();
    }
};

//--------------------------------------------------------------------------------------------------

Limits getDefaultMechanicLimits();

//--------------------------------------------------------------------------------------------------

struct MovingEyes : public EyesMechanics
{
    explicit MovingEyes(const Limits &limits = getDefaultMechanicLimits());
    ~MovingEyes() override = default;

    void setActuation(const EyesActuation &actuation);

protected:
    Limits mechanic_limits;

    void trimToMechanicalLimits();

    template <typename type_t> void trim(type_t &value, const Range &range);
};

//--------------------------------------------------------------------------------------------------

template <typename type_t> void MovingEyes::trim(type_t &value, const Range &range)
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
