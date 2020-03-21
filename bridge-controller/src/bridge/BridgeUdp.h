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

/**
 * \brief Simple UDP bridge to send and receive datagrams.
 *
 * The bridge can be used in unicast or multicast mode (depending on the constructors). In order
 * to send and transmit data:
 * 1.) Call #setup() each time the WiFi connection is (re-)established.
 * 2.) To send send data call #send(const Datagram &).
 * 3.) To receive data register a datagram receiver with #setDatagramReceiver(DatagramReceiver_t *)
 *     and call #process() in your loop.
 *
 * NOTE: The bridge does not take care of datagram sequences neither of lost datagrams,
 * but it considers simple CRC.
 *
 * Verbosity and errors logging can be adjusted by #setVerbose(bool) and #setSetLogErrorsOff(bool).
 *
 * @tparam package_payload_buffer_size the maximum payload size, for limits see \ref Payload
 */
template <uint8_t package_payload_buffer_size> struct BridgeUdp
{
    static const uint8_t PACKAGE_PAYLOAD_BUFFER_SIZE{ package_payload_buffer_size };
    using Datagram_t = Datagram<package_payload_buffer_size>;
    using DatagramReceiver_t = DatagramReceiver<package_payload_buffer_size>;

    //!< multicast or unicast
    explicit BridgeUdp(const IPAddress &destination_address = { 239, 0, 0, 1 },
              uint16_t port = 8266,
              bool multicast = true,
              bool log_to_serial = true);

    //!< unicast only
    explicit BridgeUdp(String hostname, uint16_t port = 8266, bool log_to_serial = true);

    // TODO
    // { 224, 0, 0, x } not route-able
    // { 224, 0, 0, x } not route-able, all hosts in subnet
    // { 224, 0, 1, x } route-able
    // { 239, 0, 0, 57 } port 12345


    bool setup();

    bool process();


    bool setDatagramReceiver(DatagramReceiver_t *datagram_receiver);

    /**
     * Updates the CRC and sends the data
     * @param datagram data to send
     * @return true if sent successfully, false otherwise
     */
    bool send(Datagram_t &datagram);

    //!< en-/disables logging info to serial
    void setVerbose(bool verbose_on);

    //!< dis-/enables logging errors to serial
    void setSetLogErrorsOff(bool errors_off);

private:
    DatagramReceiver_t *receiver{ nullptr };
    WiFiUDP udp;

    const bool multicast_mode;
    const IPAddress destination_address;
    const String host_name;
    const uint16_t port;

    bool log_verbose_on{ false };
    bool log_errors_off{ false };
    IPAddress local_ip{ 0, 0, 0, 0 };
    void fetchLocalIp();
};

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
BridgeUdp<package_payload_buffer_size>::BridgeUdp(const IPAddress &destination_address, uint16_t port, bool multicast, bool log_to_serial)
: multicast_mode{ multicast }, destination_address{ destination_address }, host_name{ "" }, port{ port }, log_verbose_on{ log_to_serial }
{
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
BridgeUdp<package_payload_buffer_size>::BridgeUdp(String host_name, uint16_t port, bool log_to_serial)
: multicast_mode{ false }, destination_address{ 0, 0, 0, 0 }, host_name{ std::move(host_name) }, port{ port }, log_verbose_on{ log_to_serial }
{
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size> bool BridgeUdp<package_payload_buffer_size>::setup()
{
    udp.stop();
    udp.stopAll();

    if(multicast_mode)
    {
        fetchLocalIp();

        if(log_verbose_on)
            Serial.println("BridgeUdp::setup: udp multicast");
        if(0 == udp.beginMulticast(local_ip, destination_address, port))
        {
            if(!log_errors_off)
            {
                Serial.print("BridgeUdp::setup: failed to initialize UDP multicast socket ");
                Serial.print("with local ip ");
                Serial.print(local_ip);
                Serial.print(" remote address ");
                Serial.print(destination_address);
                Serial.printf(":%d\n", port);
            }
            return false;
        }
    }
    else
    {
        if(log_verbose_on)
            Serial.println("BridgeUdp::setup: udp unicast");
        if(0 == udp.begin(port))
        {
            if(!log_errors_off)
            {
                Serial.print("BridgeUdp::setup: failed to initialize UDP unicast socket ");
                Serial.print("with remote address ");
                Serial.print(destination_address);
                Serial.printf(":%d\n", port);
            }
            return false;
        }
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
bool BridgeUdp<package_payload_buffer_size>::process()
{
    static Datagram_t datagram;
    bool processed{ false };
    const uint8_t minimum_packet_size{ static_cast<uint8_t>(
    sizeof(Datagram_t) - sizeof(BridgeUdp::PACKAGE_PAYLOAD_BUFFER_SIZE)) };

    int datagram_size{ udp.parsePacket() };
    if(datagram_size > 0)
    {
        if(log_verbose_on)
        {
            Serial.printf("BridgeUdp::process: received UDP packet size %d from ", datagram_size);
            Serial.print(udp.remoteIP());
            Serial.printf(":%d\n", udp.remotePort());
        }

        for(uint16_t len = static_cast<uint16_t >(udp.read(datagram.toUint8Ptr(), sizeof(Datagram_t))); len > 0;
            len = static_cast<uint16_t >(udp.read(datagram.toUint8Ptr(), sizeof(Datagram_t))))
        {
            if(len < minimum_packet_size)
            {
                if(log_verbose_on)
                    Serial.printf("BridgeUdp::process: "
                                  "received %d bytes but expected at least %d bytes"
                                  ", skipping those %d bytes\n",
                                  len, minimum_packet_size, len);
            }
            else
            {
                if(log_verbose_on)
                    Serial.printf("BridgeUdp::process: received %s\n", toString(datagram).c_str());

                if(!datagram.verifyCrc())
                {
                    if(!log_errors_off)
                        Serial.printf("BridgeUdp::process: failed to verify CRC of datagram %s\n",
                                      toString(datagram).c_str());
                }
                else if(nullptr != receiver)
                {
                    receiver->take(datagram);
                }
            }
            processed = true;
        }
        udp.flush();
    }

    return processed;
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
bool BridgeUdp<package_payload_buffer_size>::send(Datagram_t &datagram)
{
    if(log_verbose_on)
    {
        Serial.printf("BridgeUdp::send: sending %s datagram size %d to ",
                      (multicast_mode) ? "multicast" : "unicast", datagram.dynamicSize());
        Serial.print(destination_address);
        Serial.printf(":%d (payload %d)\n", port, datagram.package.payload.bytes_buffered);
    }

    // ----- begin packet

    if(multicast_mode)
    {
        uint8_t ttl{ 3 };
        if(0 == udp.beginPacketMulticast(destination_address, port, local_ip, ttl))
        {
            if(!log_errors_off)
                Serial.printf("BridgeUdp::send: failed to begin multicast package\n");
            return false;
        }
    }
    else
    {
        int begin_state{ 0 };

        if(host_name.isEmpty())
            begin_state = udp.beginPacket(destination_address, port);
        else
            begin_state = udp.beginPacket(host_name.c_str(), port);

        if(0 == begin_state)
        {
            if(!log_errors_off)
                Serial.printf("BridgeUdp::send: failed to begin unicast package\n");
            return false;
        }
    }

    // ----- write packet

    uint16_t old_crc = datagram.crc;
    datagram.updateCrc();

    if(0 == udp.write(datagram.toUint8Ptr(), datagram.dynamicSize()))
    {
        if(!log_errors_off)
            Serial.printf("BridgeUdp::send: failed to write %s package\n",
                          (multicast_mode) ? "multicast" : "unicast");
        datagram.crc = old_crc;
        return false;
    }

    // ----- (s)end packet

    if(0 == udp.endPacket())
    {
        if(!log_errors_off)
            Serial.printf("BridgeUdp::send: failed to end %s package\n",
                          (multicast_mode) ? "multicast" : "unicast");
        return false;
    }

    if(log_verbose_on)
        Serial.printf("BridgeUdp::send: sent datagram %s\n", toString(datagram).c_str());

    return true;
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
void BridgeUdp<package_payload_buffer_size>::setVerbose(bool verbose_on)
{
    log_verbose_on = verbose_on;
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
void BridgeUdp<package_payload_buffer_size>::setSetLogErrorsOff(bool errors_off)
{
    log_errors_off = errors_off;
}

// -------------------------------------------------------------------------------------------------

template <uint8_t package_payload_buffer_size>
void BridgeUdp<package_payload_buffer_size>::fetchLocalIp()
{
    local_ip = WiFi.softAPIP();
    // local_ip = WiFi.localIP();
}
