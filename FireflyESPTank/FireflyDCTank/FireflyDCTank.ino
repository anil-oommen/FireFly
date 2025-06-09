
/**************************************************
 * FireFly DC Motor Control Unit for Tank.
 * Version 1.0 for FireFily Remote Controllers.
 *************************************************/
const char  app_compile_date[] = __DATE__ " " __TIME__;
const char* app_compile_version = "FireFly MCU DC Tank 0.1";
#define     FIREFLY_MCU_NAME    "FIREFLY_MCU_DCTANK"


#include "WifiConnector.h"
#include "EventTimerUtils.h"
#include "MqttConnector.h"
#include "Mpu6050LibraryDMP6.h"





/* DC Tank Motor A as Left */
#define DIR_A 0
#define PWM_A 5
/* DC Tank Motor B as Right */
#define DIR_B 2
#define PWM_B 4

#define MAX_PWM_RANGE 1000

void _callbackMqttConnected(){
  Serial.println("INFO : [FIREFLY] READY : Broadcasting Notification, Ready for command ...");
  publishNotifyBootup();
}
void _callbackJoystickCommand(int x,int y){
  //Value range from -10 to +10 
  //Serial.printf("INFO : [FIREFLY] Run Motor %3d %3d\n", left,right);
  /*
  int leftDirection = (x<0)?0:1;
  int rightDirection = (x<0)?0:1;
  int leftPwmValue = ((x<0)?-1:1)  * (x/10.0)   *  MAX_PWM_RANGE;
  int righPwmValue = ((y<0)?-1:1) * (y/10.0)  *  MAX_PWM_RANGE;
  */
  int leftPwmValue = MAX_PWM_RANGE;
  int righPwmValue = MAX_PWM_RANGE;
  int leftDirection   = ((sqrt(y*y))/y)<0?0:1;
  int rightDirection  = ((sqrt(y*y))/y)<0?0:1;
  
  _callbackDCSignal(leftDirection , leftPwmValue,rightDirection, righPwmValue );
}
void _callbackDCSignal(int leftDirection, int leftPwmValue, int rightDirection, int righPwmValue){
  analogWrite(PWM_A,leftPwmValue);   digitalWrite(DIR_A,leftDirection );
  analogWrite(PWM_B,righPwmValue);   digitalWrite(DIR_B,rightDirection );
  Serial.printf("INFO : [FIREFLY] DC.MOTOR A[%3d %3d]  B[%3d %3d] \n", leftPwmValue,leftDirection,righPwmValue,rightDirection );
}
void _callbackStopMotor(){
  Serial.printf("INFO : [FIREFLY] DC.MOTOR -STOP--------------\n");
  analogWrite(PWM_A,0);   digitalWrite(DIR_A,1);
  analogWrite(PWM_B,0);   digitalWrite(DIR_B,1);
}

float   lastYaw_Data = 0;
//float   lastYaw_CalibrationYawMeanAdjusted = 0;
unsigned long lastYaw_CalibrationLastMs = 0;;
boolean lastYaw_CalibrationDone = false;
#define LASTYAW_CHANGE_SENSITIVITY  1.0
int _callbackYawPitchRoll(const float yaw, const float pitch, const float roll){
  float diffYawData = fabsf(lastYaw_Data-yaw);
  //Calibrating untill the value is settled.
  if(!lastYaw_CalibrationDone && (millis() - lastYaw_CalibrationLastMs)> 2000){ //During Calibration, interval check shoyld be long enough
    if(diffYawData < LASTYAW_CHANGE_SENSITIVITY) {
      lastYaw_CalibrationDone = true;//calibration done, things have settled down.
      Serial.println("INFO : [FIREFLY] MPU6050 Calibration Completed");
      char yawBuffer[10]; sprintf(yawBuffer, "%05.01f", yaw);
      publishEvent("SensorYaw",yawBuffer,"Calibration Complete.");
    }else{
      Serial.println("INFO : [FIREFLY] MPU6050 Calibrating..");
    }
    lastYaw_CalibrationLastMs = millis();
    lastYaw_Data = yaw ; return 0;
  }else if(lastYaw_CalibrationDone){
    if(diffYawData > LASTYAW_CHANGE_SENSITIVITY) {
      char yawBuffer[10]; sprintf(yawBuffer, "%05.01f", yaw);
      publishEvent("SensorYaw",yawBuffer,"Change");
      //Serial.printf("Tank Direction %s \n" , yawBuffer);
      lastYaw_Data = yaw ; return 1;
    }  
  }
   return 0;
}

void setup()
{
  
  Serial.begin(115200);
  Serial.println("INFO : [FIREFLY] Booting up.");
  delay(10);

  //Callback for MQTT Connection.
  CallbackMqttConnected = _callbackMqttConnected;
  CallbackJoystickCommand = _callbackJoystickCommand;
  CallbackDCSignal = _callbackDCSignal;
  CallbackStopMotor = _callbackStopMotor;
  
  //Registering Callback for MPU6050 
  CallbackMPU6050YawPitchRoll = _callbackYawPitchRoll;

  

  setupWifiConnection();

  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  
  analogWrite(PWM_A,0);
  digitalWrite(DIR_A,1);
  analogWrite(PWM_B,0);
  digitalWrite(DIR_B,1);

  delay(1000);

  if(mpu_dmp6_setup()){
    //CallbackMPU6050YawPitchRoll = _callbackYawPitchRoll;
  }
 
}

void loop(){
  loopMqttConnector();
  mpu_dmp6_loop();
  //delay(500);
}

  /*
   * 
   * 
   * 
  Serial.println("INFO : [FIREFLY] Starting motor..");
  Serial.println("Speed: 5");
  analogWrite(PWM_A,5);
  analogWrite(PWM_B,5);
  delay(5000);

  Serial.println("Speed: 50");
  analogWrite(PWM_A,50);
  analogWrite(PWM_B,50);
  delay(5000);


  Serial.println("Speed: 100");
  analogWrite(PWM_A,100);
  analogWrite(PWM_B,100);
  delay(5000);

  Serial.println("Speed: 300");
  analogWrite(PWM_A,300);
  analogWrite(PWM_B,300);
  delay(5000);


  Serial.println("Speed: 1000");
  analogWrite(PWM_A,1000);
  analogWrite(PWM_B,1000);
  delay(5000);

  Serial.println("Stop: 0");
  analogWrite(PWM_A,0);
  analogWrite(PWM_B,0);
  delay(10000);
  */

/*
 * 
 * 
 * int IN1 = D7; 
int IN2 = D8; 
int IN3 = D5;
int IN4 = D6;

void loopDisable()
{
  char user_input;
  
  while(Serial.available())
  {
    user_input = Serial.read();
    digitalWrite(IN1, LOW); //OFF
    digitalWrite(IN2, LOW);

    if (user_input =='1')
    {
       Forward();
    }
    else if(user_input =='2')
    {
      Reverse();
    }
    else if(user_input =='3')
    {
      Stop();
    }
    else
    {
      Serial.println("Invalid option entered.");
    }
  }
}


void Forward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.print("Motor 1 Forward");
  Serial.println();
  
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);  
  Serial.println("Motor 2 Forward");
  Serial.println();
}
void Reverse()
{
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, HIGH);
  Serial.print("Motor 1 Reverse");
  Serial.println();
  
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, HIGH);
  Serial.println("Motor 2 Reverse");
  Serial.println();
}

void Stop()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    Serial.print("Motor 1 Stop");
    Serial.println();
    
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    Serial.println("Motor 2 Stop");
    Serial.println();

}
*/
