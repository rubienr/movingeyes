#pragma once

#include <WString.h>

//--------------------------------------------------------------------------------------------------

struct SerialLineReader
{
    void setup();
    bool process();
    const String &getLine();
    void clear();

private:
    String buffered_line;
};