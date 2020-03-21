#pragma once

#include <WString.h>
#include <cstdint>
#include <CRC.h>

// -------------------------------------------------------------------------------------------------

template <uint8_t buffer_size> struct Payload
{
    //!< On AVR based boards, outgoing UDP packets are limited to 72 bytes in size currently.
    //!< For non-AVR boards the limit is 1446 bytes.
    //!< See: https://www.arduino.cc/en/Reference/WiFiUDPConstructor
    uint8_t bytes_buffered{ 0 };
    uint8_t data[buffer_size]{ 0 };

    uint8_t dynamicSize() const { return (buffer_size - bytes_buffered); }
};

// -------------------------------------------------------------------------------------------------

struct Header
{
    enum class Type : uint8_t
    {
        DATA,
        COMMAND
    };
    Type type{ Type::DATA };

    uint8_t dynamicSize() const { return static_cast<uint8_t>(sizeof(Header)); }
};

// -------------------------------------------------------------------------------------------------

template <uint8_t payload_buffer_size> struct Package
{
    using Payload_t = Payload<payload_buffer_size>;

    Header header;
    Payload_t payload;

    uint8_t *toUint8Ptr() { return reinterpret_cast<uint8_t *>(this); }
    const uint8_t *toUint8Ptr() const { return reinterpret_cast<const uint8_t *>(this); }

    uint8_t dynamicSize() const { return header.dynamicSize() + payload.dynamicSize(); }
};


// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size> struct Datagram
{
    using Package_t = Package<package_payload_buffer_size>;
    using CrcType = uint16_t;

    CrcType crc{ 0 };
    Package_t package;

    void fromBytes(void *bytes) { memcpy(this, bytes, sizeof(Datagram)); }


    uint8_t *toUint8Ptr() { return reinterpret_cast<uint8_t *>(this); }
    const uint8_t *toUint8Ptr() const { return reinterpret_cast<const uint8_t *>(this); }

    /**
     * @return the size of the package considering the buffered/valid bytes
     */
    uint8_t dynamicSize() const
    {
        return static_cast<uint8_t>(sizeof(CrcType) + package.dynamicSize());
    }

    void updateCrc() { crc = CRC::crc16(package.toUint8Ptr(), dynamicSize()); }

    bool verifyCrc() { return crc == CRC::crc16(package.toUint8Ptr(), dynamicSize()); }
};

// -------------------------------------------------------------------------------------------------

template <uint8_t buffer_size> String toString(const Payload<buffer_size> &p)
{
    String s{ "Payload: {bytes_buffered: " };
    s.concat(p.bytes_buffered);
    s.concat("}, data: ");

    for(uint8_t i = 0; i < p.bytes_buffered; i++)
        s.concat(static_cast<char>(p.data[i]));

    s.concat('}');
    return s;
}

// -------------------------------------------------------------------------------------------------

String toString(const Header &h)
{
    String s{ "Header: {type: " };
    s.concat(static_cast<uint8_t>(h.type));
    s.concat('}');
    return s;
}

// -------------------------------------------------------------------------------------------------

template <uint8_t payload_buffer_size> String toString(const Package<payload_buffer_size> &p)
{
    String s{ "Package: {" };
    s.concat(toString(p.header));
    s.concat(", ");
    s.concat(toString(p.payload));
    s.concat('}');
    return s;
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
String toString(const Datagram<package_payload_buffer_size> &d)
{
    String s{ "Datagram: {" };
    s.concat("crc: ");
    s.concat(d.crc);
    s.concat(", ");
    s.concat(toString(d.package));
    s.concat('}');
    return s;
}
