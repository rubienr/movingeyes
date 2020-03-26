#include "utils.h"
#include <BridgeUdp.h>
#include <HardwareSerial.h>
#include <SerialReaderUdp.h>
#include <UdpReceiverSerial.h>
#include <pinutils.h>

//--------------------------------------------------------------------------------------------------

constexpr const uint16_t MESSAGE_PAYLOAD_BUFFER = 255;
using UdpReceiverSerial_t = UdpReceiverSerial<MESSAGE_PAYLOAD_BUFFER>;
using BridgeUdp_t = BridgeUdp<MESSAGE_PAYLOAD_BUFFER>;
using SerialReaderUdp_t = SerialReaderUdp<MESSAGE_PAYLOAD_BUFFER>;

//--------------------------------------------------------------------------------------------------

struct Resources {
  //----------------------------------------------------------------------------------------------

  TogglePin led_pin{LED_BUILTIN};
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
      led_pin.toggle();
      Serial.printf("WIFI not connected, mode %s\n",
                    (utils::isWifiModeAp()) ? "AP" : "STA");
      delay(1000);
      return;
    }

    if (utils::isWifiReconnected()) {
      led_pin.toggle();
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
      led_pin.toggle();

    // read serial -> send to UDP
    if (serial_reader.process())
      led_pin.toggle();
  }
} r;
