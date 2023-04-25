/**
 * Code that will be running in the control unit of the UR
 * 
 * It needs to control the motor gripper and the pneumatic one under commands from the robotic arm. 
 * It also needs to be able to read and process distances from a ToF sensor, and send and receive data from the UR.
 * 
*/

#include <Arduino.h>
#include <math.h>
#include "common.h"
#include "classic_gripper.h"

/******* Generic initializations  *******/
/// @brief Possible configurations for the TOF sensor
enum TOFMODE
{
  POSITIONING = 0,
  DETECTING = 1
};

/// @brief Available grippers
enum GRIPPER
{
  CLASSIC = 0,
  PNEUMATIC = 1
};


// State variables
GRIPPER gripper_type = CLASSIC;
TOFMODE tof_mode = POSITIONING;

// TOF variables
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;
bool DISTANCE_THRESHOLD_SIGNAL;

/****************************************/


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

/// @brief Function that reads the value from the TOF sensor and sends a digital signal 
/// wether the distance falls under certain threshold or not.
/// @return 
Error read_TOF(GRIPPER gripper_type, TOFMODE tof_mode)
{
  // Distance in mm to send signal
  int threshold = 0;

  switch(gripper_type){
    case CLASSIC:
      if(tof_mode == POSITIONING) // Positioning with classic gripper
        threshold = 300;
      if(tof_mode == DETECTING)   // Detecting with classic gripper
        threshold = 250;
    break;
    case PNEUMATIC:
      if(tof_mode == POSITIONING) // Positioning with pneumatic gripper
        threshold = 300;
      if(tof_mode == DETECTING)   // Detecting with pneumatic gripper
        threshold = 250;
    break;
  }

  // Reading of the TOF using the libraries
  int distance = lox.readRange();
 

  // Send signal if measured distance is closer than threshold
  if(distance < threshold)
    DISTANCE_THRESHOLD_SIGNAL = true;
  else
    DISTANCE_THRESHOLD_SIGNAL = false;

  Serial.println(distance);

  return Error::OK;
}

void setup() 
{
  /******* Declaration of pin modes *******/


  // Classic Gripper
  // pinMode(cg::PIN_CURRENTSENSOR, INPUT);
  // pinMode(cg::PIN_MOTOR, OUTPUT);
  // pinMode(cg::PIN_TOF_IN, INPUT);
  pinMode(cg::PIN_TOF_OUT, OUTPUT);

  DISTANCE_THRESHOLD_SIGNAL = false;

  /****************************************/
  // TOF activation
    if (!lox.begin()) {
      Serial.println(F("Failed to boot VL53L0X"));
      while(1);
    }
  lox.begin();

  Serial.begin(9600);
}

void loop() 
{
  //cg::close_gripper();
  read_TOF(CLASSIC, POSITIONING);
  digitalWrite(cg::PIN_TOF_OUT, DISTANCE_THRESHOLD_SIGNAL);
  // Serial.print("Signal sent: ");
  // Serial.println(DISTANCE_THRESHOLD_SIGNAL);

}