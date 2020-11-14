#pragma once

#include "Event.h"

namespace funduino {

//--------------------------------------------------------------------------------------------------

struct EventReceiver {
    virtual bool take(const ShieldEvent &e) = 0;
};

} // namespace funduino