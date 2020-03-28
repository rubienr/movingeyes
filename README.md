Work in progress ...

# Monika (Moving Eyes) 
Implementation for Printed Animatronic Eye Mechanism. Lets call it Monika!

* https://www.instructables.com/id/DIY-Compact-3D-Printed-Animatronic-Eye-Mechanism/
* https://www.youtube.com/watch?v=Ftt9e8xnKE4  

This project consists of two parts
1. [The controller](controller/README.md) (wired mode), and the 
1. [Wireless Bridge](wifi-bridge/README.md) (optional, wifi mode)

## Overview
### Wired
```
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
### Wireless
Uses a UDP bridge in multicast mode to transmit data. 
```

  _______                ______             _________
 | Eyes  |              | PCA  |           | Wemos-  |
 | mech. | ←--6x PWM--- | 9685 | ←--i2c--- | D1 mini | 
 |_______|              |______|           |_________|
                                                 ↑
                                                 |
                                            📶 Wifi UDP 
                                                 |
  ________              ________             ____↓____
 | Joyst. |            | Ardu-  |           | Wemos   |
 | shield | ---pins--→ | ino L. | ---i2c--→ | D1 mini |
 |________|            |________|           |_________|
```
