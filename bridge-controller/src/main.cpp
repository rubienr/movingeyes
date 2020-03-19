#include "bridge/UdpReceiverSerial.h"
#include "WiFiUdp.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <HardwareSerial.h>
#include <bridge/BridgeUdp.h>
#include <pinutils.h>


//--------------------------------------------------------------------------------------------------
using BridgeUdp_t = BridgeUdp<255>;
using UdpReceiverSerial_t = UdpReceiverSerial<255>;

namespace
{

//--------------------------------------------------------------------------------------------------

struct SerialLineBuffer
{
    bool readLine()
    {
        while(Serial.available() > 0)
        {
            char c;
            if(1 == Serial.read(&c, 1))
            {
                if((c == '\r' || c == '\n') && buffered_line.length() != 0) return true;
                buffered_line.concat(c);
            }
        }

        return false;
    }

    String buffered_line;
};

} // namespace

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

    SerialLineBuffer serial_buffer;

    bool server_mode{ false };
    bool last_wifi_connected_state{ false };

    BridgeUdp_t bridge;
    UdpReceiverSerial_t datagram_receiver_serial;

    //----------------------------------------------------------------------------------------------

    void setup()
    {
        wifi_status_led_uninstall();
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH); // off

#ifdef MODE_SERVER
        server_mode = true;
#elif MODE_CLIENT
        server_mode = false;
#else
#error WiFi mode not defined (WIFI_AP or WIFI_STA).
#endif

        String ssid{ WIFI_SSID };
        String passphrase{ WIFI_PASSPHRASE };

        if(server_mode)
        {
            Serial.println("WiFi mode AP");
            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid, passphrase);
            IPAddress ip(192, 168, 0, 1);
            IPAddress gw(192, 168, 0, 1);
            IPAddress subnet(255, 255, 255, 0);
            WiFi.config(ip, gw, subnet);
            // WiFi.enableAP(true);
            digitalWrite(LED_BUILTIN, HIGH); // off
        }
        else
        {
            Serial.println("WiFi mode STA");
            WiFi.mode(WIFI_STA);
            // WiFi.setAutoConnect(true);
            // WiFi.enableSTA(true);
            WiFi.begin(ssid, passphrase);
            digitalWrite(LED_BUILTIN, LOW); // on
        }

        WiFi.setOutputPower(10);
        Serial.println("WiFi OK");
        Serial.print("Local IP ");
        Serial.println(WiFi.localIP());

        bridge.setup();
        datagram_receiver_serial.setup();
        bridge.setDatagramReceiver(&datagram_receiver_serial);
    }

    //----------------------------------------------------------------------------------------------

    bool isWifiReconnected()
    {
        if(!last_wifi_connected_state && isWifiConnected()) { last_wifi_connected_state = true; }
        else
        {
            last_wifi_connected_state = false;
        }

        return last_wifi_connected_state;
    }

    //----------------------------------------------------------------------------------------------

    bool isWifiConnected()
    {
        if(server_mode && WiFi.softAPgetStationNum() > 0) { return true; }

        if(!server_mode && WiFi.isConnected()) { return true; }

        return false;
    }

    //----------------------------------------------------------------------------------------------

    void process()
    {
        // sanity check
        if(!isWifiConnected())
        {
            led_pin.toggle();
            delay(1000);
            return;
        }

        if(isWifiReconnected())
        {
            led_pin.toggle();
            if(server_mode) Serial.println("AP reconnected");
            else
                Serial.println("STA connected");

            Serial.print("Local IP ");
            Serial.println(WiFi.localIP());
        }

        // read udp -> send to serial
        bridge.process();


        // read serial -> write to udp
        if(serial_buffer.readLine())
        {
            Serial.println("Serve serial");
            BridgeUdp_t ::Datagram_t datagram;

            memcpy(datagram.package.payload.data, serial_buffer.buffered_line.c_str(),
                   serial_buffer.buffered_line.length());
            datagram.package.payload.bytes_buffered = serial_buffer.buffered_line.length();

            bridge.send(datagram);
            led_pin.toggle();
        }
    }

} r;

//--------------------------------------------------------------------------------------------------

void setup() { r.setup(); }

//--------------------------------------------------------------------------------------------------

void loop() { r.process(); }
