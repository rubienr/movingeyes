#pragma once

#include <WString.h>

//--------------------------------------------------------------------------------------------------

struct SerialLineReader
{
    /**
     * Allows to ex-/include the newline character in the line string.
     * With additional newline it is possible to receive also empty lines.
     *
     * @param inclusive_newline_character whether or not to include the '\n' in the line string.
     */
    SerialLineReader(bool inclusive_newline_character = true);

    void setup();

    /**
     * Reads from serial and accumulates bytes to buffer.
     * NOTE: Skips '\r' but takes '\n'.
     *
     * @return true if bytes have been read from serial and buffered, false otherwise
     */
    bool process();

    /**
     * Searches the buffer for the next '\n' character to determine a whole line.
     * NOTE: #process() must be called to accumulate the line buffer.
     *
     * @return true if a whole line is buffered, false otherwise
     */
    bool hasLine();

    /**
     * Retrieves the next line and removes it from buffer.
     *
     * @return the next line if #hasLine() returns true, empty string otherwise
     */
    String getLine();

    /**
     * Clear the whole lines buffer.
     */
    void clear();

private:
    String buffered_lines;
    bool inclusive_newline_character;
};