#include "Event.h"

#include "../configuration.h"

namespace funduino {

//--------------------------------------------------------------------------------------------------

void ShieldEventHelper::print(const ShieldEvent &shield_event, const String &prefix) {
    String se{ prefix };

    se.concat("key=");
    se.concat(stringFromKeyType(shield_event.key));
    se.concat("(");
    se.concat(uint8FromKeyType(shield_event.key));
    se.concat(")");

    se.concat(" event=");
    se.concat(stringFromEventType(shield_event.event));
    se.concat("(");
    se.concat(uint8FromEventType(shield_event.event));
    se.concat(") ");

    se.concat("shield_state:{");
    Serial.print(se.c_str());
    ShieldStateHelper::print(shield_event.shield_state, "");
    Serial.print("}");
}

//--------------------------------------------------------------------------------------------------

void ShieldEventHelper::println(const ShieldEvent &shield_event, const String &prefix) {
    print(shield_event, prefix);
    Serial.println();
}

//--------------------------------------------------------------------------------------------------

String stringFromEventType(const KeyEventType &et) {
    String names[]{ "pressed", "released", "changed", "none" };

    return names[uint8FromEventType(et)].c_str();
}

} // namespace funduino