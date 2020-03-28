**Project is stalled due to an issue with espressif8266 + I2C slave**
* No data is received with Wemos D1 mini in I2C slave mode:
  1. Master sends data to slave:
     1. master sends address to slave
     1. slave must acknowledge by pulling clock signal (which it doesn't)
     1. on Wire.endTransmission() master reports "received NACK on transmit of address"
* https://github.com/suculent/esp8266-I2C-slave
  * latest expressif8266 arduino framework ist 2.4.0 (https://github.com/platformio/platform-espressif8266)
* Example to reproduce issue: https://www.arduino.cc/en/Tutorial/MasterWriter

# Wireless Bridge
A bridge allowing to easily connect two endpoints via UDP. Currently it can bypass 
* serial data, and hopefully will be capable of connecting 
* I2C bus.
Unfortunately the project ist stalled due to arduino framwork + Wemos D1 mini issue.
 

## Required Hardware
* 2x WEMOS D1 Mini for bridging serial or I2C bus

## Build & Flash
### Serial Example
```
cd wifi-bridge
./clion-init.sh

# attach two Wemods D1 mini to your pc
./build-and-monitor.sh server_serial

# in a second shell 
./build-and-monitor.sh client_serial

```

The script will build, upload and monitor. 
Once the upload is finished, feel free type in the console and press enter. 
The data will be transmitted to the other endpoint.
```
  ________                ________             
 | Wemos  |              | Wemos  |
 | WiFi   | ←-- WiFi --→ | WiFi   | 
 | AP     |              | STA    |          
 |________|              |________|
```
### I2C Example
Connect following pins of both controllers:
* D1 to D1
* D2 to D2
* GND to GND

```
cd wifi-bridge
./clion-init.sh

# attach two Wemods D1 mini to your pc
./build-and-monitor.sh server_i2c

# in a second shell 
./build-and-monitor.sh client_i2c

```
Once the upload is finished, feel free type in the I2C master console and press enter. 
The data will be transmitted to the I2C slave.
```
  ________                ________             
 | Wemos  |              | Wemos  |
 | I2C    | ←-- WiFi --→ | I2C    | 
 | master |              | slave  |          
 |________|              |________|
```
