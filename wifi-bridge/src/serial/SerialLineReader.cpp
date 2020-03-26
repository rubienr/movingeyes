#include "SerialLineReader.h"
#include <HardwareSerial.h>

// -------------------------------------------------------------------------------------------------

SerialLineReader::SerialLineReader(bool inclusive_newline_character)
: inclusive_newline_character{ inclusive_newline_character }
{
}

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
                continue;

            buffered_lines.concat(c);
            return true;
        }
    }
    return false;
}

// -------------------------------------------------------------------------------------------------

String SerialLineReader::getLine()
{
    int next_newline_idx{ buffered_lines.indexOf('\n') };

    if(next_newline_idx < 0)
        return {};
    else if(next_newline_idx == 0)
    {
        buffered_lines.remove(0, 1);
        if(inclusive_newline_character)
            return { "\n" };
        else
            return {};
    }
    else // if(next_newline_idx > 0)
    {
        String next_line{ buffered_lines.substring(0, next_newline_idx) };
        buffered_lines.remove(0, next_newline_idx);

        if(!inclusive_newline_character)
            next_line.remove(next_line.length() - 1, 1);

        return next_line;
    }
}

// -------------------------------------------------------------------------------------------------

void SerialLineReader::clear() { buffered_lines.clear(); }

// -------------------------------------------------------------------------------------------------

bool SerialLineReader::hasLine() { return 0 <= buffered_lines.indexOf('\n'); }
