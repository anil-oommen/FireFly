

TODO , adding Gyro to the DC Tank Project ==============================================
MPU-6050 Accelerometer + Gyro (1 x Gyro Module)

https://www.electronicwings.com/nodemcu/mpu6050-interfacing-with-nodemcu

Digital Motion Processor
One of the really nice and powerful features of this module is its integrated Digital Motion Processor, than can do accelerometres and gyros data integration as well as low pass filtering. For that you'll need the MPU6050_DMP module.

https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6_ESPWiFi/MPU6050_DMP6_ESPWiFi.ino


		Download the entire contents of the i2cdevlib repository: https://github.com/jrowberg/i2cdevlib/archive/master.zip
		Unzip the downloaded file
		(In the Arduino IDE) Sketch > Include Library > Add .ZIP Library > select the folder i2cdevlib-master/Arduino/MPU6050 > Open

PID Library for Auto Correction using Gyro Data
http://playground.arduino.cc/Code/PIDLibrary
https://playground.arduino.cc/Code/PIDAutotuneLibrary?utm_source=rb-community&utm_medium=blog&utm_campaign=using-the-arduino-pid-library-for-precise-position-control-of-x-and-y-axis-on-repscrap-printer

https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050

http://wired.chillibasket.com/2015/03/pid-controller/
The PID Controller
PID stands for proportional, integral and derivative, referring to the mathematical equations used to calculate the output.