#include "Key.h"

namespace Funduino
{

//--------------------------------------------------------------------------------------------------

String stringFromKeyType(const KeyType &kt)
{
    String names[]{ "A", "B", "C", "D", "E", "F", "X", "Y", "Z", "__KeysCount", "Undefined" };

    return names[uint8FromKeyType(kt)];
}
} // namespace Funduino
