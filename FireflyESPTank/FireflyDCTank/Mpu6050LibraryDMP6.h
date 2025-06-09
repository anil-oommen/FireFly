#ifndef Mpu6050LibraryDMP6_h
#define Mpu6050LibraryDMP6_h

/* 
 *  Uses DMP : Digital Motion Processing instead of Basic MPU Data.
 *  NOTE: If Error in MPU Read. Power off fully and restart.
 *  
 * Code Sourced from "MPU6050_DMP6_ESPWiFi.ino" 
 * https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/examples/MPU6050_DMP6_ESPWiFi/MPU6050_DMP6_ESPWiFi.ino
 */
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include "Wire.h"

EventTimer dmpControlledRead("DMPControlledRead#",50, true, false); //every x millisecs

typedef int (*functMPU6050YawPitchRoll) (const float yaw, const float pitch, const float roll);
functMPU6050YawPitchRoll CallbackMPU6050YawPitchRoll = NULL;

const uint8_t Wire_SCL = D6;
const uint8_t Wire_SDA = D7;
#define       MPU_Interrupt_PIN D8 // use pin 15 on ESP8266



MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

boolean mpu_dmp6_setup()
{
  Wire.begin(Wire_SDA, Wire_SCL);
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
 
  // initialize device
  Serial.println(F("INFO : [MPU_DMP] Initializing I2C devices..."));
  mpu.initialize();
  pinMode(MPU_Interrupt_PIN, INPUT);

  // verify connection
  Serial.println(F("INFO : [MPU_DMP] Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("INFO : [MPU_DMP] MPU6050 connection successful") : F("ERROR: [MPU_DMP] MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("INFO : [MPU_DMP] Initializing"));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("INFO : [MPU_DMP] Enabling "));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.printf("INFO : [MPU_DMP] Enabling Interrupt detection on PIN %d\n",MPU_Interrupt_PIN);
    attachInterrupt(digitalPinToInterrupt(MPU_Interrupt_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("INFO : [MPU_DMP] Ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
    //Serial.printf("Packet Size %d\n",packetSize);
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    // NOTE : Try disconnecting : Power drain
    Serial.print(F("ERROR: [MPU_DMP] DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(") pause ||")); 
    delay(5000);
  }
  return dmpReady;
}

void mpu_dmp6_loop()
{
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  if (!mpuInterrupt && fifoCount < packetSize) return;

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  // If our code is slow, the fifoData Accumulates , wait all fifData to be purged
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // Serial.printf("Fifo Size %d\n",fifoCount);
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("WARN : [MPU_DMP] FIFO overflow! Purged."));
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    if(dmpControlledRead.isDueForRun()){
        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        if(CallbackMPU6050YawPitchRoll != NULL){ 
          CallbackMPU6050YawPitchRoll(ypr[0] * 180/M_PI, ypr[1] * 180/M_PI , ypr[2] * 180/M_PI);
        }else{
          Serial.println("ERROR 'CallbackMPU6050YawPitchRoll' not registered, dumping data..");
          Serial.print("ypr\t");
          Serial.print(ypr[0] * 180/M_PI);
          Serial.print("\t");
          Serial.print(ypr[1] * 180/M_PI);
          Serial.print("\t");
          Serial.println(ypr[2] * 180/M_PI);
        }
        
    }
  }
}







#endif
