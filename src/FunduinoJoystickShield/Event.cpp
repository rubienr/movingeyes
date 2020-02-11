#include "Event.h"

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

void Funduino::ShieldEventHelper::print(const Funduino::ShieldEvent &shield_event)
{
    Serial.println("ShieldEventHelper::print");
    String se;

    for(uint8_t idx = 0; idx < ShieldEvent::KEYS_COUNT; idx++)
    {
        const auto &key_event = shield_event.key_event[idx];
        se.concat("key=");
        se.concat(stringFromKeyType(keyTypeFromUint8(idx)));
        se.concat("(");
        se.concat(uint8FromKeyType(keyTypeFromUint8(idx)));
        se.concat(")");

        se.concat(" event=");
        se.concat(stringFromEventType(key_event));
        se.concat("(");
        se.concat(uint8FromEventType(key_event));
        se.concat(")\n");
    }

    Serial.println(se.c_str());
}

//--------------------------------------------------------------------------------------------------

String stringFromEventType(const Funduino::KeyEventType &et)
{
    String names[]{ "pressed", "released", "changed", "none" };

    return names[uint8FromEventType(et)].c_str();
}
} // namespace Funduino