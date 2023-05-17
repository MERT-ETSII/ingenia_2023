#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include "pinout.h"
#include "common.h"

TOFMODE common::get_tof_configuration()
{
  bool operation_control = digitalRead(common::PIN_OPERATION_CONTROL);
  return operation_control ? TOFMODE::DETECTING : TOFMODE::POSITIONING;
}

ERROR common::read_TOF(GRIPPER gripper_type, TOFMODE tof_mode)
{
  if(!TOF_AVAILABLE)
    TOF_AVAILABLE = lox.begin();
  if(!TOF_AVAILABLE)
    return ERROR::TOF_UNAVAILABLE;

  // Distance in mm to send signal
  int threshold = 0;

  switch(gripper_type){
    case GRIPPER::CLASSIC:
      if(tof_mode == TOFMODE::POSITIONING) // Positioning with classic gripper
        threshold = 300;
      if(tof_mode == TOFMODE::DETECTING)   // Detecting with classic gripper
        threshold = 250;
    break;
    case GRIPPER::PNEUMATIC:
      if(tof_mode == TOFMODE::POSITIONING) // Positioning with pneumatic gripper
        threshold = 300;
      if(tof_mode == TOFMODE::DETECTING)   // Detecting with pneumatic gripper
        threshold = 250;
    break;
  }

  // Reading of the TOF using the libraries
  int distance = lox.readRange();
 

  // Send signal if measured distance is closer than threshold
  DISTANCE_THRESHOLD_SIGNAL = distance < threshold;

  Serial.println("[TOF]: ");
  Serial.println(distance);

  return ERROR::OK;
}

GRIPPER common::get_gripper_type()
{
  bool gripper_signal = digitalRead(common::PIN_GRIPPER_TYPE);
  return gripper_signal ? GRIPPER::PNEUMATIC : GRIPPER::CLASSIC;
}