#pragma once

#include <bridge/BridgeUdp.h>
#include <cstdint>

// -------------------------------------------------------------------------------------------------

template <uint8_t payload_buffer_size>
struct UdpReceiverSerial : public DatagramReceiver<payload_buffer_size>
{
    void setup();

    void take(const Datagram<payload_buffer_size> &datagram) override;
};

// -------------------------------------------------------------------------------------------------

template <uint8_t payload_buffer_size> void UdpReceiverSerial<payload_buffer_size>::setup() {}

// -------------------------------------------------------------------------------------------------

template <uint8_t payload_buffer_size>
void UdpReceiverSerial<payload_buffer_size>::take(const Datagram<payload_buffer_size> &datagram)
{
    uint8_t str[payload_buffer_size + 1]{ 0 };
    memcpy(str, datagram.package.payload.data, datagram.package.payload.bytes_buffered);
    Serial.printf("->%s<-\n", str);
}
