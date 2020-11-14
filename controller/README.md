# Monika (Moving Eyes) 
Source code for Printed Animatronic Eye Mechanism. Lets call it Monika!

* https://www.instructables.com/id/DIY-Compact-3D-Printed-Animatronic-Eye-Mechanism/
* https://www.youtube.com/watch?v=Ftt9e8xnKE4  

## Required Hardware
* 1x Arduino Leonardo (or compatible)
* 1x Funduino Joystick Shield (or compatible)
* 1x PCA9685 16x-12bit pwm (or compatible)
* 6x Micro servos (i.e. MG90S)
* 1x set of 3D printed eye mechanics + screws
* optionally 2x WEMOS D1 Mini for wireless bridge in between the Joystick Shield and Arduino, and PCA8695 

## Build & Flash & Run
```
cd base-board
./clion-init.sh
pio run --target upload
```
After power up a joystick calibration is required:
 1. turn the joystick to each axis' min/max limit 
 2. leave the joystick in its idle position 

## Pinout & Wiring
```
Wired Setup
  _______                ___________             ____________
 | Eyes  |              | PCA 9685  |           |  Arduino   |
 | mech. | ←--6x PWM--- |           | ←--i2c--- |            | 
 |       |              | I2C slave |           | I2C master |          
 |_______|              |___________|           |____________|
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

### Wireless (optional)
TBD: Work in progress ...
TBD: Work stalled because of bug - arduino framework + UDP issue
```
Wireless Setup
  _______                ___________             ____________
 | Eyes  |              | PCA 9685  |           | Wemos-     |
 | mech. | ←--6x PWM--- |           | ←--i2c--- | D1 mini    | 
 |       |              | I2C slave |           | I2C master |          
 |_______|              |___________|           |____________|
                                                      ↑
                                                      |
                                                 📶 Wifi UDP
                                                      |
  ________              ____________             _____↓_____
 | Joyst. |            | Ardu-      |           | Wemos     |
 | shield | ---pins--→ | ino L.     | ---i2c--→ | D1 mini   |
 |        |            | I2C master |           | I2C slave |
 |________|            |____________|           |___________|
```
