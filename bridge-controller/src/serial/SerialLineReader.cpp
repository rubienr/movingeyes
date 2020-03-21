#include "SerialLineReader.h"
#include <HardwareSerial.h>

// -------------------------------------------------------------------------------------------------

void SerialLineReader::setup() {}

// -------------------------------------------------------------------------------------------------

bool SerialLineReader::process()
{
    while(Serial.available() > 0)
    {
        char c;
        if(1 == Serial.read(&c, 1))
        {
            if(c == '\r')
                return false;

            if(c == '\n')
                return true;

            buffered_line.concat(c);
        }
    }

    return false;
}

// -------------------------------------------------------------------------------------------------

const String &SerialLineReader::getLine() { return buffered_line; }

// -------------------------------------------------------------------------------------------------

void SerialLineReader::clear() { buffered_line.clear(); }
