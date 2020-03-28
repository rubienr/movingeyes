#ifdef BRIDGE_SERIAL
#include "serial.h"
#endif

#ifdef BRIDGE_I2C
#include "i2c.h"
#endif

#include "utils.h"

//--------------------------------------------------------------------------------------------------

// sanity check

#ifdef BRIDGE_SERIAL
#pragma message "CONNECTION MODE: SERIAL"
#elif BRIDGE_I2C
#pragma message "CONNECTION MODE: I2C"
#else
#error Either BRIDGE_SERIAL or BRIDGE_I2C must be defined.
#endif

#ifdef MODE_SERVER
#pragma message "SERVICE MODE: SERVER"
#elif MODE_CLIENT
#pragma message "SERVICE MODE: CLIENT"
#else
#error Either MODE_SERVER or MODE_CLIENT must be defined.
#endif

//--------------------------------------------------------------------------------------------------


#ifdef BRIDGE_I2C
void onBytesReceived(int bytes_count);
void onRequest(void);
#endif

//--------------------------------------------------------------------------------------------------

struct Ressources
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

            // todo rr
#ifdef BRIDGE_I2C
        if (!utils::isServerMode())
        {
            Wire.onReceive(onBytesReceived);
            Wire.onRequest(onRequest);
        }
#endif
        }
    } _;

    //----------------------------------------------------------------------------------------------

    TogglePin led{ LED_BUILTIN };

#ifdef BRIDGE_SERIAL
    UdpSerial connection;
#elif BRIDGE_I2C
    UdpWire connection;
#endif

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

        if(utils::isServerMode())
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

        // WiFi.setOutputPower(10);
        Serial.println("WiFi OK");
        Serial.print("Local IP ");
        Serial.println(WiFi.localIP());
        Serial.print("SoftAP IP ");
        Serial.println(WiFi.softAPIP());

        connection.setup();
    }

    void process() { connection.process(); }

} r;

//--------------------------------------------------------------------------------------------------

void setup() { r.setup(); }

//--------------------------------------------------------------------------------------------------

void loop() { r.process(); }

//--------------------------------------------------------------------------------------------------

#ifdef BRIDGE_I2C
void onBytesReceived(int bytes_count) { r.connection.onBytesReceived(bytes_count); }
void onRequest() { r.connection.onRequest(); }
#endif
