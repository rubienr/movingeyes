#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>

#include <SerialLineReader.h>
#include <pinutils.h>

#include "serial.h"
#include "utils.h"

//--------------------------------------------------------------------------------------------------

// extern void onBytesReceived(int bytes_count);

//--------------------------------------------------------------------------------------------------

struct UdpWire
{
    //----------------------------------------------------------------------------------------------

    TogglePin led{ LED_BUILTIN };
    SerialLineReader line_reader;

    uint8_t address{ (utils::isServerMode()) ? 1 : 2 };
    uint8_t other_address{ (utils::isServerMode()) ? 2 : 1 };

    String slave_recv_buffer;

    //----------------------------------------------------------------------------------------------

    void setup()
    {
        Serial.printf("UdpWire::setup: our I2C address is %d\n", address);
        Wire.setClock(50000);
        Wire.begin(SDA, SCL, address);

        line_reader.setup();
    }

    //----------------------------------------------------------------------------------------------

    // I2C -> serial
    void onBytesReceived(int bytes_count)
    {
        // Serial.printf("UdpWire::onBytesReceived: received %d bytes >", bytes_count);

        while(Wire.available() > 0)
        {
            slave_recv_buffer.concat(Wire.read());
        }
    }

    //----------------------------------------------------------------------------------------------

    void onRequest() { Serial.printf("UdpWire::onRequest\n"); }

    //----------------------------------------------------------------------------------------------

    // serial -> I2C
    void process()
    {
        line_reader.process();

        if(utils::isServerMode())
        {
            while(line_reader.hasLine())
            {
                String line{ line_reader.getLine() };

                Wire.beginTransmission(other_address);

                size_t bytes_written = Wire.write(line.c_str());
                if(bytes_written != line.length())
                {
                    Serial.printf("UdpWire::process: failed to write line bytes: wrote only %d bytes of %d\n",
                                  bytes_written, line.length());
                    Wire.flush();
                    return;
                }

                uint8_t send_result = Wire.endTransmission();
                switch(send_result)
                {
                case 0: // ok
                    led.toggle();
                    return;
                    break;

                case 2:
                    Serial.printf("UdpWire::process: received NACK on transmit of address %d\n", other_address);
                    Wire.flush();
                    break;

                case 3:
                    Serial.println("UdpWire::process: received NACK on transmit of data");
                    Wire.flush();
                    break;

                case 4:
                    Serial.println("line busy");
                    Wire.flush();
                    break;
                }
            }
        }
        else
        {
            while(!slave_recv_buffer.isEmpty())
            {
                Serial.printf("UdpWire::process: I2C receive buffer >%s<\n", slave_recv_buffer.c_str());
                slave_recv_buffer = "";
            }
        }
    }

    void scan()
    {
        uint8_t count = 0;
        for(byte addr = 8; addr < 120; addr++)
        {
            Wire.beginTransmission(addr);
            if(Wire.endTransmission() == 0)
            {
                Serial.print("Found I2C Device: ");
                Serial.print(" (0x");
                Serial.print(addr, HEX);
                Serial.println(")");
                count++;
                delay(1);
            }
        }
    }
};
