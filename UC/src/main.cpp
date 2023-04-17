/**
 * Code that will be running in the control unit of the UR
 * 
 * It needs to control the motor gripper and the pneumatic one under commands from the robotic arm. 
 * It also needs to be able to read and process distances from a ToF sensor, and send and receive data from the UR.
 * 
*/

#include <Arduino.h>
#include<math.h>
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
    voltage_sensor = (analogRead(cg::PIN_CURRENTSENSOR) - 553.2731257)/31.17464354;
    current += voltage_sensor;
  }
  current = current/n_samples;
  return(current);
}

Error cg::close_gripper()
{
  float I=cg::get_current(200); //Read current over 200 samples

  // TO DO: Don't use delays
  digitalWrite(cg::PIN_MOTOR, HIGH);  // Run motor 500 ms
  delay(500);

  unsigned long init_time = millis();
  while(fabs(I) < cg::CURRENT_THRESHOLD) // Still moving
  {
    // If operation takes too long, there is probably an error. 
    // Stop the motor and exit
    if(millis() - init_time > cg::MAX_CLOSING_TIME_MILLIS)
      {
        digitalWrite(cg::PIN_MOTOR, LOW);  // Stop motor
        return Error::TIMEOUT;
      }
  }

  digitalWrite(cg::PIN_MOTOR, LOW);  // Stop motor
  return Error::OK;
}

void setup() 
{
  /******* Declaration of pin modes *******/

  // Classic Gripper
  pinMode(cg::PIN_CURRENTSENSOR, INPUT);
  pinMode(cg::PIN_MOTOR, OUTPUT);

  /***************************************/

  Serial.begin(9600);
}

void loop() 
{
  cg::close_gripper();
}