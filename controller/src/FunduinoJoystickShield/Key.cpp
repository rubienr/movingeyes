#include "Key.h"

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

String stringFromKeyType(const KeyType &kt)
{
    String names[]{ "A", "B", "C", "D", "E", "F", "X", "Y", "Z", "__KeysCount", "Void" };

    return names[uint8FromKeyType(kt)];
}

//--------------------------------------------------------------------------------------------------

String stringFromKeyStateType(const KeyStateType &ks)
{
    String names[]{ "pressed", "released", "Undefined" };

    return names[uint8FromKeyState(ks)];
}
} // namespace Funduino
