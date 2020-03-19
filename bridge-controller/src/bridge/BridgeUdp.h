#pragma once

#include "types.h"
#include <ESP8266WiFi.h>
#include <HardwareSerial.h>
#include <WiFiUdp.h>
#include <cstdint>
#include <string.h>

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size> struct DatagramReceiver
{
    virtual void take(const Datagram<package_payload_buffer_size> &datagram) = 0;
};

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size> struct BridgeUdp
{
    using Datagram_t = Datagram<package_payload_buffer_size>;
    using DatagramReceiver_t = DatagramReceiver<package_payload_buffer_size>;

    BridgeUdp(IPAddress broadcast_address = { 224, 0, 1, 3 }, uint16_t port = 8266);

    void setup();

    void process();

    bool setDatagramReceiver(DatagramReceiver_t *datagram_receiver);

    void send(Datagram_t datagram);

private:
    DatagramReceiver_t *receiver{ nullptr };
    WiFiUDP udp;

    IPAddress broadcast_address{ 224, 0, 1, 3 };
    uint16_t port{ 8266 };
};

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
BridgeUdp<package_payload_buffer_size>::BridgeUdp(IPAddress broadcast_address, uint16_t port)
: broadcast_address{ broadcast_address }, port{ port }
{
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size> void BridgeUdp<package_payload_buffer_size>::setup()
{
    udp.beginMulticast(WiFi.localIP(), broadcast_address, port);
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
void BridgeUdp<package_payload_buffer_size>::process()
{
    static Datagram_t datagram;
    const uint8_t minimum_packet_size{ static_cast<uint8_t>(
    sizeof(Datagram_t) - sizeof(Datagram_t::Package_t::Payload_t::BUFFER_SIZE)) };

    if(udp.parsePacket() > 0)
    {
        for(uint16_t len = udp.read(datagram.toUint8Ptr(), sizeof(Datagram_t)); len > 0;
            len = udp.read(datagram.toUint8Ptr(), sizeof(Datagram_t)))
        {
            if(len < minimum_packet_size)
            {
                Serial.printf("received %d bytes but expected at least %d bytes,"
                              " skipping those bytes\n",
                              len, minimum_packet_size);
            }
            else
            {
                if(nullptr != receiver) { receiver->take(datagram); }
            }
        };
        Serial.println();
    }
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
bool BridgeUdp<package_payload_buffer_size>::setDatagramReceiver(DatagramReceiver_t *datagram_receiver)
{
    if(receiver != datagram_receiver)
    {
        receiver = datagram_receiver;
        return true;
    }

    receiver = datagram_receiver;
    return false;
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
void BridgeUdp<package_payload_buffer_size>::send(Datagram_t datagram)
{
    udp.beginPacketMulticast(broadcast_address, port, WiFi.localIP());
    udp.write(datagram.package.payload.data, datagram.package.payload.bytes_buffered);
    udp.endPacket();
}
