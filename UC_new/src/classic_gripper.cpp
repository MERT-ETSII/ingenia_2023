#define DEBUG_CURRENTSENSOR 1

#include <Arduino.h>
#include "pinout.h"
#include "classic_gripper.h"

float cg::CURRENT_THRESHOLD = 0.4;

unsigned long cg::MAX_CLOSING_TIME_MILLIS = 3000;

unsigned int cg::MIN_TIME_MILLIS = 500;

unsigned int cg::PWM_VALUE=200;

float cg::get_current(int n_samples)
{

  float voltage;
  float current = 0;
  long analog_acum = 0;
  for(int i=0;i<n_samples;i++){
    int analog_lecture = analogRead(PIN_CURRENTSENSOR);
    analog_acum += analog_lecture;
  }
  
  float analog_lecture = (float)analog_acum/(float)n_samples;
  // Source: trust me, bro
  voltage = analog_lecture/1023*5;
  current = (voltage - 2.5)/0.2;
  if(DEBUG_CURRENTSENSOR){
    Serial.print("[CURRENT_S]: Analog: ");
    Serial.println(analog_lecture);
    Serial.print("[CURRENT_S]: Voltage: ");
    Serial.println(voltage);
    Serial.print("[CURRENT_S]: Current: ");
    Serial.println(current);
  }
  return(current);
}

cg::UR_ACTION cg::get_action(){
  bool action_requested = digitalRead(PIN_GRIPPER_ACTION);
  return action_requested ? cg::UR_ACTION::OPEN_GRIPPER : cg::UR_ACTION::CLOSE_GRIPPER;  
}

ERROR cg::close_gripper()
{
  float I=cg::get_current(200); //Read current over 200 samples

  unsigned long init_start_time = millis();
  while(millis() - init_start_time > cg::MIN_TIME_MILLIS)
  {
    digitalWrite(PIN_STBY, HIGH); // Enable motor
    digitalWrite(PIN_MOTOR_PWM, PWM_VALUE); // No PWM. Full pawah

    digitalWrite(PIN_MOTOR_1, HIGH);  // Run motor 
    digitalWrite(PIN_MOTOR_2, LOW);  // Run motor 
  }

  unsigned long init_time = millis();
  while(fabs(I) < cg::CURRENT_THRESHOLD) // Still moving
  {
    I=cg::get_current(200); //Read current over 200 samples

    // If operation takes too long, there is probably an error. 
    // Stop the motor and exit
    if(millis() - init_time > cg::MAX_CLOSING_TIME_MILLIS)
      {
        digitalWrite(PIN_STBY, LOW); // Disable motor
        digitalWrite(PIN_MOTOR_1, LOW);  // Stop motor 
        digitalWrite(PIN_MOTOR_2, LOW);  // Stop motor 
        return ERROR::TIMEOUT;
      }
  }

  digitalWrite(PIN_STBY, LOW); // Disable motor
  digitalWrite(PIN_MOTOR_1, LOW);  // Stop motor 
  digitalWrite(PIN_MOTOR_2, LOW);  // Stop motor 
  return ERROR::OK;
}

ERROR cg::open_gripper()
{
  //if(cg::)
  float I=cg::get_current(200); //Read current over 200 samples
  
  unsigned long init_start_time = millis();
  while(millis() - init_start_time > cg::MIN_TIME_MILLIS)
  {
    digitalWrite(PIN_STBY, HIGH); // Enable motor
    digitalWrite(PIN_MOTOR_PWM, PWM_VALUE); // No PWM. Full pawah

    digitalWrite(PIN_MOTOR_1, HIGH);  // Run motor 
    digitalWrite(PIN_MOTOR_2, LOW);  // Run motor 
  }

  unsigned long init_time = millis();
  while(fabs(I) < cg::CURRENT_THRESHOLD) // Still moving
  {
    I=cg::get_current(200); //Read current over 200 samples

    // If operation takes too long, there is probably an error. 
    // Stop the motor and exit
    if(millis() - init_time > cg::MAX_CLOSING_TIME_MILLIS)
      {
        digitalWrite(PIN_STBY, LOW); // Disable motor
        digitalWrite(PIN_MOTOR_1, LOW);  // Stop motor 
        digitalWrite(PIN_MOTOR_2, LOW);  // Stop motor 
        return ERROR::TIMEOUT;
      }
  }

  digitalWrite(PIN_STBY, LOW); // Disable motor
  digitalWrite(PIN_MOTOR_1, LOW);  // Stop motor 
  digitalWrite(PIN_MOTOR_2, LOW);  // Stop motor 
  return ERROR::OK;
}