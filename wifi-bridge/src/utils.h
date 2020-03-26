#include <Arduino.h>
#include <ESP8266WiFi.h>

namespace utils
{

//----------------------------------------------------------------------------------------------

bool isWifiModeAp() { return WiFi.getMode() == WIFI_AP; }

//----------------------------------------------------------------------------------------------

bool isWifiConnected()
{
    if(isWifiModeAp() && WiFi.softAPgetStationNum() > 0)
    {
        return true;
    }

    if(!isWifiModeAp() && WiFi.isConnected())
    {
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------

bool isWifiReconnected()
{
    static bool last_wifi_connected_state{ false };

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

} // namespace utils