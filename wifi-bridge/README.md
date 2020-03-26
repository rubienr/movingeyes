# Wireless Bridge 

## Required Hardware
* 2x WEMOS D1 Mini for bridging an I2C bus

## Build & Flash
```
cd base-board
./clion-init.sh
pio run --target upload
```

## Pinout & Wiring
```
Wired Setup
  _______                ______             ________
 | Eyes  |              | PCA  |           | Ardu-  |
 | mech. | ←--6x PWM--- | 9685 | ←--i2c--- | ino L. | 
 |_______|              |______|           |________|
                                               ↑
                                               |
                                           shield pins
                                            ___|____
                                           | Joyst. |
                                           | shield |
                                           |________|
```
### Funduino Joystick Shield => Arduino Leonardo
1. Remove pins of keys A and B from joystick shield since they interfer with the I2C bus. 
1. Bypass the same pins:
   1. A -> D9
   1. B -> D10
1. Set the switch to 5V.
1. Stack the joystick shield onto Arduino Leonardo.

### Arduino Leonardo => PCA9685 12-bit PWM
1. connect Arduino SCL -> PCA9685 SCL
1. connect Arduino SDA -> PCA9685 SDA

### Funduino Joystick => PCA9685 12-bit PWM
1. Joystick GND -> PCA9685 GND
1. Joystick 5V -> PCA9685 V+

### PCA9685 => Servos
Servos are as from Monika's perspective seen:
1. PCA9685 0 -> bearing
1. PCA9685 1 -> elevation
1. PCA9685 2 -> left upper lid
1. PCA9685 3 -> left lower lid
1. PCA9685 4 -> right upper lid
1. PCA9685 5 -> right lower lid

### Wireless (opional)
TBD: Work in progress ...
```
Wireless Setup
  _______                ______             _________
 | Eyes  |              | PCA  |           | Wemos-  |
 | mech. | ←--6x PWM--- | 9685 | ←--i2c--- | D1 mini | 
 |_______|              |______|           |_________|
                                                 ↑
                                                 |
                                              Wifi UDP 📶 📶
                                                 |
  ________              ________             ____↓____
 | Joyst. |            | Ardu-  |           | Wemos   |
 | shield | ---pins--→ | ino L. | ---i2c--→ | D1 mini |
 |________|            |________|           |_________|
```