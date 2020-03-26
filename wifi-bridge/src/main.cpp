#ifdef BRIDGE_SERIAL
#include "serial.h"
#elif BRIDGE_I2C
#include "i2c.h"
#else
#error Either BRIDGE_I2C or BRIDGE_SERIAL must be defined.
#endif

//--------------------------------------------------------------------------------------------------

extern Resources r;

//--------------------------------------------------------------------------------------------------

struct Setup
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
            Serial.println("Setup::setup");
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

    BridgeUdp_t bridge{ { 239, 0, 0, 1 }, UDP_PORT, true };

    UdpReceiverSerial_t datagram_receiver_serial;

    //----------------------------------------------------------------------------------------------

    void setup()
    {
        wifi_status_led_uninstall();
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH); // off

        Serial.printf("clock %dMHz\n", ESP.getCpuFreqMHz());
        Serial.printf("boot version %d boot mode %d\n", ESP.getBootVersion(), ESP.getBootMode());
        Serial.printf("sdk  ver. %s\n", ESP.getSdkVersion());
        Serial.printf("core ver. %s\n", ESP.getCoreVersion().c_str());
        Serial.printf("full ver. %s\n", ESP.getFullVersion().c_str());

        String ssid{ WIFI_SSID };
        String passphrase{ WIFI_PASSPHRASE };

        if(server_mode)
        {
            Serial.println("WiFi mode AP");
            WiFi.enableAP(false);
            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid, passphrase);
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
            WiFi.enableSTA(true);
            digitalWrite(LED_BUILTIN, LOW); // on
        }

        //WiFi.setOutputPower(10);
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
} s;

//--------------------------------------------------------------------------------------------------

void setup()
{
    s.setup();
    r.setup();
}

//--------------------------------------------------------------------------------------------------

void loop() { r.process(); }
