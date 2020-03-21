#include "bridge/UdpReceiverSerial.h"
#include "serial/SerialLineReader.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <HardwareSerial.h>
#include <bridge/BridgeUdp.h>
#include <pinutils.h>

//--------------------------------------------------------------------------------------------------

constexpr const uint8_t MESSAGE_PAYLOAD_BUFFER = 255;
using BridgeUdp_t = BridgeUdp<MESSAGE_PAYLOAD_BUFFER>;
using UdpReceiverSerial_t = UdpReceiverSerial<MESSAGE_PAYLOAD_BUFFER>;

//--------------------------------------------------------------------------------------------------

struct Resources
{
    //----------------------------------------------------------------------------------------------
    struct EarlyInit
    {
        EarlyInit()
        {

            Serial.begin(230400);
            while(!Serial)
                delay(10);
            Serial.print("\n\n\n");
            Serial.println("Resources::setup");
        }
    } _;

    //----------------------------------------------------------------------------------------------

    TogglePin led_pin{ LED_BUILTIN };

    SerialLineReader serial_buffer;

#ifdef MODE_SERVER
    bool server_mode{ true };
#elif MODE_CLIENT
    bool server_mode{ false };
#endif

    bool last_wifi_connected_state{ false };

#ifdef MODE_SERVER
    // BridgeUdp_t bridge{ { 192, 169, 0, 2 }, 8266, false };
#elif MODE_CLIENT
    // BridgeUdp_t bridge{ { 192, 169, 0, 1 }, 8266, false };
#endif

    BridgeUdp_t bridge{ { 239, 0, 0, 1 }, UDP_PORT, true };

    UdpReceiverSerial_t datagram_receiver_serial;

    //----------------------------------------------------------------------------------------------

    void setup()
    {
        wifi_status_led_uninstall();
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH); // off

        delay(500);

        Serial.printf("clock %dMHz\n", ESP.getCpuFreqMHz());
        Serial.printf("boot version %d boot mode %d\n", ESP.getBootVersion(), ESP.getBootMode());
        Serial.printf("sdk  ver. %s\n", ESP.getSdkVersion());
        Serial.printf("core ver. %s\n", ESP.getCoreVersion().c_str());
        Serial.printf("full ver. %s\n", ESP.getFullVersion().c_str());

        String ssid{ WIFI_SSID };
        String passphrase{ WIFI_PASSPHRASE };
        // IPAddress gw(192, 168, 0, 1);
        // IPAddress subnet(255, 255, 255, 0);

        if(server_mode)
        {

            Serial.println("WiFi mode AP");
            WiFi.enableAP(false);
            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid, passphrase);
            // IPAddress ip(192, 168, 0, 1);
            // WiFi.config(ip, gw, subnet);
            WiFi.enableAP(true);
            digitalWrite(LED_BUILTIN, HIGH); // off
        }
        else
        {
            Serial.println("WiFi mode STA");
            WiFi.enableSTA(false);
            WiFi.mode(WIFI_STA);
            WiFi.begin(ssid, passphrase);
            WiFi.setAutoConnect(true);
            // IPAddress ip(192, 168, 0, 2);
            // WiFi.config(ip, gw, subnet);
            WiFi.enableSTA(true);
            digitalWrite(LED_BUILTIN, LOW); // on
        }

        WiFi.setOutputPower(10);
        Serial.println("WiFi OK");
        Serial.print("Local IP ");
        Serial.println(WiFi.localIP());
        Serial.print("SoftAP IP ");
        Serial.println(WiFi.softAPIP());

        datagram_receiver_serial.setup();
        bridge.setVerbose(false);
        bridge.setSetLogErrorsOff(false);
        bridge.setup();
        bridge.setDatagramReceiver(&datagram_receiver_serial);
    }

    //----------------------------------------------------------------------------------------------

    bool isWifiReconnected()
    {
        if(isWifiConnected())
        {
            if(!last_wifi_connected_state)
            {
                last_wifi_connected_state = true;
                return true;
            }
        }
        else
        {
            last_wifi_connected_state = true;
            return false;
        }

        return false;
    }

    //----------------------------------------------------------------------------------------------

    bool isWifiConnected()
    {
        if(server_mode && WiFi.softAPgetStationNum() > 0)
        {
            return true;
        }

        if(!server_mode && WiFi.isConnected())
        {
            return true;
        }

        return false;
    }

    //----------------------------------------------------------------------------------------------

    void process()
    {
        // sanity check
        if(!isWifiConnected())
        {
            led_pin.toggle();
            Serial.printf("WIFI not connected, mode %s\n", (server_mode) ? "AP" : "STA");
            delay(1000);
            return;
        }

        if(isWifiReconnected())
        {
            led_pin.toggle();
            Serial.printf("WIFI connected, mode %s\n", (server_mode) ? "AP" : "STA");

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
            BridgeUdp_t ::Datagram_t datagram;
            const String &line{ serial_buffer.getLine() };

            if(line.length() <= BridgeUdp_t::PACKAGE_PAYLOAD_BUFFER_SIZE)
            {
                memcpy(datagram.package.payload.data, line.c_str(), line.length());
                datagram.package.payload.bytes_buffered = static_cast<uint8_t >(line.length());
                bridge.send(datagram);
            }

            serial_buffer.clear();
            led_pin.toggle();
        }
    }

} r;

//--------------------------------------------------------------------------------------------------

void setup() { r.setup(); }

//--------------------------------------------------------------------------------------------------

void loop() { r.process(); }
