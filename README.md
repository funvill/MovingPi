# MovingPi
A python script for a Raspberry PI that plays a sound effect when it is moved. 

Sound effect 
-------------

- Charel Sytze, [onweer08063.mp3](https://www.freesound.org/people/Charel%20Sytze/sounds/36063/), [Attribution 3.0 Unported (CC BY 3.0)](https://creativecommons.org/licenses/by/3.0/)

Hardware
-------------

- [Raspberry PI 3](https://www.raspberrypi.org/)
- [MPU-6050](https://www.sparkfun.com/products/11028) - SparkFun Triple Axis Accelerometer and Gyro Breakout 

| RPi         | MPU-6050 | Arduino | WS2812  |
| ----------- | -------- | --------| --------|
| GND         |      GND |     GND |     GND |
| 3.3v        |      VCC |         |         |
| SCL/PIN #2  |      SCL |         |         |
| SDA/PIN #3  |      SDA |         |         |
| PIN #24     |          |   Pin 5 |         |
|             |          |   Pin 6 | Data IN |
|             |          |      5V |      5V |


![Raspberry pi diagram](https://raw.githubusercontent.com/funvill/MovingPi/master/rpi_drawing_bb.png)

Heartbeat
-------------

[Video of heartbeat LEDs](https://www.instagram.com/p/BTDbGdrBdWt/)
