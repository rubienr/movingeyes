#pragma once

#include <cstdint>

// -------------------------------------------------------------------------------------------------

template <uint8_t buffer_size> struct Payload
{
    static const uint8_t BUFFER_SIZE{ buffer_size };
    //!< the amount of valid bytes in #data
    uint8_t bytes_buffered{ 0 };
    uint8_t data[buffer_size]{ 0 };
};

// -------------------------------------------------------------------------------------------------

struct Header
{
    enum class Type
    {
        DATA,
        COMMAND
    };
    Type type{ Type::DATA };
};

// -------------------------------------------------------------------------------------------------

template <uint8_t payload_buffer_size> struct Package
{
    using Payload_t = Payload<payload_buffer_size>;

    Header header;
    Payload_t payload;
};

// -------------------------------------------------------------------------------------------------
template <uint8_t package_payload_buffer_size>

struct Datagram
{
    using Package_t = Package<package_payload_buffer_size>;
    uint8_t crc{ 0 };
    Package_t package;

    void fromBytes(void *bytes) { memcpy(this, bytes, sizeof(Datagram)); }

    void *toVoidPtr() { return static_cast<void *>(this); }
    uint8_t *toUint8Ptr() { return reinterpret_cast<uint8_t *>(this); }

    /**
     * @return the complete packatge size without respecting buffered state
     */
    uint8_t staticSize() { return static_cast<uint8_t>(sizeof(Datagram)); }

    /**
     * @return the size of the package considering the buffered/valid bytes
     */
    uint8_t dynamicSize()
    {
        return static_cast<uint8_t>(
        sizeof(Datagram) - (Package_t::Payload_t::BUFFER_SIZE - package.payload.bytes_buffered));
    }
};


