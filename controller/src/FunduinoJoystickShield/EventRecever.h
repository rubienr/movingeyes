#pragma once

#include "Event.h"

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

struct EventReceiver
{
    virtual bool take(const Funduino::ShieldEvent &e) = 0;
};

} // namespace Funduino