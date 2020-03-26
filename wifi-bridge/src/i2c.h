#include "serial/SerialLineReader.h"
#include "utils.h"
#include <Arduino.h>
#include <BridgeUdp.h>
#include <HardwareSerial.h>
#include <UdpReceiverSerial.h>
#include <pinutils.h>

//--------------------------------------------------------------------------------------------------

constexpr const uint16_t MESSAGE_PAYLOAD_BUFFER = 255;
using BridgeUdp_t = BridgeUdp<MESSAGE_PAYLOAD_BUFFER>;
using UdpReceiverSerial_t = UdpReceiverSerial<MESSAGE_PAYLOAD_BUFFER>;

//--------------------------------------------------------------------------------------------------

struct Resources
{
    //----------------------------------------------------------------------------------------------

    TogglePin led_pin{ LED_BUILTIN };
    SerialLineReader serial_buffer;
    BridgeUdp_t bridge{ { 239, 0, 0, 1 }, UDP_PORT, true };
    UdpReceiverSerial_t datagram_receiver_serial;

    //----------------------------------------------------------------------------------------------

    void setup()
    {
        datagram_receiver_serial.setup();
        bridge.setVerbose(false);
        bridge.setSetLogErrorsOff(false);
        bridge.setup();
        bridge.setDatagramReceiver(&datagram_receiver_serial);
    }

    //----------------------------------------------------------------------------------------------

    void process()
    {
        // sanity check
        if(!utils::isWifiConnected())
        {
            led_pin.toggle();
            Serial.printf("WIFI not connected, mode %s\n", (utils::isWifiModeAp()) ? "AP" : "STA");
            delay(1000);
            return;
        }

        if(utils::isWifiReconnected())
        {
            led_pin.toggle();
            Serial.printf("WIFI connected, mode %s\n", (utils::isWifiModeAp()) ? "AP" : "STA");

            Serial.print("Local  IP ");
            Serial.println(WiFi.localIP());
            Serial.print("SoftAP IP ");
            Serial.println(WiFi.softAPIP());
            bridge.setup();
        }

        // read udp -> send to serial
        if(bridge.process())
        {
            led_pin.toggle();
        }

        // read serial -> write to udp
        if(serial_buffer.process())
        {
            while(serial_buffer.hasLine())
            {
                String line{ serial_buffer.getLine() };

                static BridgeUdp_t ::Datagram_t datagram;
                if(line.length() <= sizeof(datagram.package.payload.data))
                {
                    uint16_t bytes_count{ static_cast<uint16_t>(line.length()) };

                    memcpy(datagram.package.payload.data, line.c_str(), bytes_count);
                    datagram.package.payload.bytes_buffered = bytes_count;
                    datagram.updateCrc();

                    bridge.send(datagram);
                }
                led_pin.toggle();
            }
        }
    }

} r;
