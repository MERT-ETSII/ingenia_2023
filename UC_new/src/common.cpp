#define DEBUG_TOF 0

#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include "pinout.h"
#include "common.h"

bool tof::DISTANCE_THRESHOLD_SIGNAL;
bool tof::TOF_AVAILABLE;
Adafruit_VL53L0X tof::lox; 
VL53L0X_RangingMeasurementData_t tof::measure; 

tof::TOFMODE tof::get_tof_configuration()
{
  bool operation_control = digitalRead(PIN_OPERATION_CONTROL);
  return operation_control ? tof::TOFMODE::DETECTING : tof::TOFMODE::POSITIONING;
}

ERROR tof::read_TOF(type::GRIPPER gripper_type, tof::TOFMODE tof_mode)
{
  if(!tof::TOF_AVAILABLE)
    tof::TOF_AVAILABLE = tof::lox.begin();
  if(!tof::TOF_AVAILABLE)
    return ERROR::TOF_UNAVAILABLE;

  // Distance in mm to send signal
  int threshold = 0;

  switch(gripper_type){
    case type::GRIPPER::CLASSIC:
    {
      if(tof_mode == TOFMODE::POSITIONING) // Positioning with classic gripper
        threshold = 300;
      if(tof_mode == TOFMODE::DETECTING)   // Detecting with classic gripper
        threshold = 250;
      break;
    }
    case type::GRIPPER::PNEUMATIC:
    {
      if(tof_mode == TOFMODE::POSITIONING) // Positioning with pneumatic gripper
        threshold = 300;
      if(tof_mode == TOFMODE::DETECTING)   // Detecting with pneumatic gripper
        threshold = 250;
      break;
    }
  }

  // Reading of the TOF using the libraries
  int distance = tof::lox.readRange();
 

  // Send signal if measured distance is closer than threshold
  tof::DISTANCE_THRESHOLD_SIGNAL = distance < threshold;

  if(DEBUG_TOF){
    Serial.println("[TOF]: ");
    Serial.println(distance);
  }

  return ERROR::OK;
}

type::GRIPPER type::get_gripper_type()
{
  bool gripper_signal = digitalRead(PIN_GRIPPER_TYPE);
  return gripper_signal ? GRIPPER::PNEUMATIC : GRIPPER::CLASSIC;
}