#include <HardwareSerial.h>

#include <UdpReceiverSerial.h>
#include <BridgeUdp.h>
#include <SerialReaderUdp.h>

#include <pinutils.h>
#include "utils.h"

//--------------------------------------------------------------------------------------------------

// todo rr: move bridge to main and pass ref to here
constexpr const uint16_t MESSAGE_PAYLOAD_BUFFER = 255;
using UdpReceiverSerial_t = UdpReceiverSerial<MESSAGE_PAYLOAD_BUFFER>;
using BridgeUdp_t = BridgeUdp<MESSAGE_PAYLOAD_BUFFER>;
using SerialReaderUdp_t = SerialReaderUdp<MESSAGE_PAYLOAD_BUFFER>;

//--------------------------------------------------------------------------------------------------

struct UdpSerial {
  //----------------------------------------------------------------------------------------------

  TogglePin led{LED_BUILTIN};
  UdpReceiverSerial_t datagram_receiver_serial;
  BridgeUdp_t bridge{{239, 0, 0, 1}, UDP_PORT, true};
  SerialReaderUdp_t serial_reader;

  //----------------------------------------------------------------------------------------------

  void setup() {
    datagram_receiver_serial.setup();

    bridge.setVerbose(false);
    bridge.setSetLogErrorsOff(false);
    bridge.setup();
    bridge.setDatagramReceiver(&datagram_receiver_serial);

    serial_reader.setup();
    serial_reader.setDatagramReceiver(&bridge);
  }

  //----------------------------------------------------------------------------------------------

  void process() {
    // sanity check
    if (!utils::isWifiConnected()) {
      led.toggle();
      Serial.printf("WIFI not connected, mode %s\n",
                    (utils::isWifiModeAp()) ? "AP" : "STA");
      delay(1000);
      return;
    }

    if (utils::isWifiReconnected()) {
      led.toggle();
      Serial.printf("WIFI connected, mode %s\n",
                    (utils::isWifiModeAp()) ? "AP" : "STA");

      Serial.print("Local  IP ");
      Serial.println(WiFi.localIP());
      Serial.print("SoftAP IP ");
      Serial.println(WiFi.softAPIP());
      bridge.setup();
    }

    // read UDP -> send to serial
    if (bridge.process())
      led.toggle();

    // read serial -> send to UDP
    if (serial_reader.process())
      led.toggle();
  }
};
