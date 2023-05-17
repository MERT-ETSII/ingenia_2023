#include <Arduino.h>

#include "pinout.h"
#include "classic_gripper.h"

float cg::get_current(int n_samples)
{
  // Current in Amperes: (analog reading - 553.2731257)/31.17464354
  // for an analog reading in range 0 - 1023
  // Source: trust me, bro

  float voltage_sensor;
  float current = 0;
  for(int i=0;i<n_samples;i++)
  {
    voltage_sensor = (analogRead(PIN_CURRENTSENSOR) - 553.2731257)/31.17464354;
    current += voltage_sensor;
  }
  current = current/n_samples;
  return(current);
}

cg::UR_ACTION cg::get_action(){
  bool action_requested = digitalRead(PIN_GRIPPER_ACTION);
  return action_requested ? cg::UR_ACTION::OPEN_GRIPPER : cg::UR_ACTION::CLOSE_GRIPPER;  
}

ERROR cg::close_gripper()
{
  float I=cg::get_current(200); //Read current over 200 samples

  // TO DO: Don't use delays
  digitalWrite(PIN_STBY, HIGH); // Enable motor
  digitalWrite(PIN_MOTOR_PWM, HIGH); // No PWM. Full pawah

  digitalWrite(PIN_MOTOR_1, HIGH);  // Run motor 
  digitalWrite(PIN_MOTOR_2, LOW);  // Run motor 
  delay(500); // Run 500 ms to avoid current sensor noise

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
  // TO DO: Don't use delays
  digitalWrite(PIN_STBY, HIGH); // Enable motor
  digitalWrite(PIN_MOTOR_PWM, HIGH); // No PWM. Full pawah

  digitalWrite(PIN_MOTOR_1, LOW);  // Run motor 
  digitalWrite(PIN_MOTOR_2, HIGH);  // Run motor 
  delay(500); // Run 500 ms to avoid current sensor noise

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