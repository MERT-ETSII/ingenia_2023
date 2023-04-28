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

// State variables
GRIPPER gripper_type = GRIPPER::CLASSIC;
TOFMODE tof_mode = TOFMODE::POSITIONING;

// TOF variables
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure;
bool DISTANCE_THRESHOLD_SIGNAL;
bool TOF_AVAILABLE;

/****************************************/

UC_MODE common::read_ur_task()
{
  bool is_tof_requested = digitalRead(common::PIN_TOF_IN);
  // Check the rest of the inputs and program state logic
  // ...

  if(is_tof_requested)
    return UC_MODE::SEND_TOF_SIGNAL;
  else
    return UC_MODE::IDLE;

}

float cg::get_current(int n_samples)
{
  // Current in Amperes: (analog reading - 553.2731257)/31.17464354
  // for an analog reading in range 0 - 1023
  // Source: trust me, bro

  float voltage_sensor;
  float current = 0;
  for(int i=0;i<n_samples;i++)
  {
    voltage_sensor = (analogRead(common::PIN_CURRENTSENSOR) - 553.2731257)/31.17464354;
    current += voltage_sensor;
  }
  current = current/n_samples;
  return(current);
}

Error cg::close_gripper()
{
  float I=cg::get_current(200); //Read current over 200 samples

  // TO DO: Don't use delays
  digitalWrite(common::PIN_MOTOR, HIGH);  // Run motor 500 ms
  delay(500);

  unsigned long init_time = millis();
  while(fabs(I) < cg::CURRENT_THRESHOLD) // Still moving
  {
    // If operation takes too long, there is probably an error. 
    // Stop the motor and exit
    if(millis() - init_time > cg::MAX_CLOSING_TIME_MILLIS)
      {
        digitalWrite(common::PIN_MOTOR, LOW);  // Stop motor
        return Error::TIMEOUT;
      }
  }

  digitalWrite(common::PIN_MOTOR, LOW);  // Stop motor
  return Error::OK;
}

Error common::read_TOF(GRIPPER gripper_type, TOFMODE tof_mode)
{
  if(!TOF_AVAILABLE)
    TOF_AVAILABLE = lox.begin();
  if(!TOF_AVAILABLE)
    return Error::TOF_UNAVAILABLE;

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

  return Error::OK;
}

void setup() 
{
  /******* Declaration of pin modes *******/


  // Classic Gripper
  pinMode(common::PIN_CURRENTSENSOR, INPUT);
  pinMode(common::PIN_MOTOR, OUTPUT);
  pinMode(common::PIN_TOF_IN, INPUT);
  pinMode(common::PIN_TOF_OUT, OUTPUT);

  DISTANCE_THRESHOLD_SIGNAL = false;

  /****************************************/
  // TOF activation
  Serial.begin(9600);
  
  TOF_AVAILABLE = lox.begin();

}

void loop() 
{
  // Update the state of the UC
  UC_MODE uc_mode = common::read_ur_task();

  // Initialise output variable
  Error output = Error::NO_INSTRUCTION;

  // Perform actions depending on state
  switch(uc_mode)
  {
    // Idle
    case UC_MODE::IDLE:
      Serial.println("Idling...");
      break;

    // Request of distance from UR
    case UC_MODE::SEND_TOF_SIGNAL:
      Serial.println("Reading TOF");
      output = common::read_TOF(GRIPPER::CLASSIC, TOFMODE::POSITIONING);

      if(output == Error::TOF_UNAVAILABLE)
        Serial.println("TOF sensor unavailable");
      if(output == Error::OK)
        digitalWrite(common::PIN_TOF_OUT, DISTANCE_THRESHOLD_SIGNAL);
      break;

    // Request of close gripper from UR
    case UC_MODE::CLOSE_GRIPPER:
      Serial.println("Closing gripper");
      output = cg::close_gripper();

      if(output == Error::OK)
        Serial.println("Gripper closed");
      if(output == Error::TIMEOUT)
        Serial.println("TIMEOUT - Stop closing");
      break;

    // Request of open gripper from UR
    case UC_MODE::OPEN_GRIPPER:
      Serial.println("Not implemented");
      break;

    // Default state (should not happen)
    default:
      Serial.println("ERROR");
      break;

  }
}