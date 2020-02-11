
#include "MovingEyes.h"
#include "EyesMechanics.h"

namespace EyeMech
{

//--------------------------------------------------------------------------------------------------

MovingEyes::MovingEyes(const Limits &limits) : mechanic_limits(limits) {}

//--------------------------------------------------------------------------------------------------

void MovingEyes::setActuation(const EyesActuation &eyes_actuation)
{
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

Limits getDefaultMechanicLimits()
{
    Limits l;

    l.bearing = { -50, 50 };
    l.elevation = { -50, 50 };
    l.left.lid.upper = { -50, 50 };
    l.left.lid.lower = { -50, 50 };
    l.left.lid.upper = { -50, 50 };
    l.left.lid.lower = { -50, 50 };

    l.autoComplete();
    return l;
}

} // namespace EyeMech