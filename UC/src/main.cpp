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
GRIPPER gripper_type = GRIPPER::CLASSIC;  // Selected gripper
TOFMODE tof_mode = TOFMODE::POSITIONING;  // TOF configuration
cg::UR_ACTION classic_gripper_state = cg::UR_ACTION::OPEN_GRIPPER;  // To keep track of the gripper position

// TOF variables
Adafruit_VL53L0X lox = Adafruit_VL53L0X(); // TOF driver
VL53L0X_RangingMeasurementData_t measure;  // TOF measured distance
bool DISTANCE_THRESHOLD_SIGNAL;
bool TOF_AVAILABLE;

/****************************************/

TOFMODE common::get_tof_configuration()
{
  bool operation_control = digitalRead(common::PIN_OPERATION_CONTROL);
  return operation_control ? TOFMODE::DETECTING : TOFMODE::POSITIONING;
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

cg::UR_ACTION cg::get_action(){
  bool action_requested = digitalRead(common::PIN_GRIPPER_ACTION);
  return action_requested ? cg::UR_ACTION::OPEN_GRIPPER : cg::UR_ACTION::CLOSE_GRIPPER;  
}

ERROR cg::close_gripper()
{
  float I=cg::get_current(200); //Read current over 200 samples

  // TO DO: Don't use delays
  digitalWrite(common::PIN_STBY, HIGH); // Enable motor
  digitalWrite(common::PIN_MOTOR_PWM, HIGH); // No PWM. Full pawah

  digitalWrite(common::PIN_MOTOR_1, HIGH);  // Run motor 
  digitalWrite(common::PIN_MOTOR_2, LOW);  // Run motor 
  delay(500); // Run 500 ms to avoid current sensor noise

  unsigned long init_time = millis();
  while(fabs(I) < cg::CURRENT_THRESHOLD) // Still moving
  {
    I=cg::get_current(200); //Read current over 200 samples

    // If operation takes too long, there is probably an error. 
    // Stop the motor and exit
    if(millis() - init_time > cg::MAX_CLOSING_TIME_MILLIS)
      {
        digitalWrite(common::PIN_STBY, LOW); // Disable motor
        digitalWrite(common::PIN_MOTOR_1, LOW);  // Stop motor 
        digitalWrite(common::PIN_MOTOR_2, LOW);  // Stop motor 
        return ERROR::TIMEOUT;
      }
  }

  digitalWrite(common::PIN_STBY, LOW); // Disable motor
  digitalWrite(common::PIN_MOTOR_1, LOW);  // Stop motor 
  digitalWrite(common::PIN_MOTOR_2, LOW);  // Stop motor 
  return ERROR::OK;
}

ERROR cg::open_gripper()
{
  //if(cg::)
  float I=cg::get_current(200); //Read current over 200 samples
  // TO DO: Don't use delays
  digitalWrite(common::PIN_STBY, HIGH); // Enable motor
  digitalWrite(common::PIN_MOTOR_PWM, HIGH); // No PWM. Full pawah

  digitalWrite(common::PIN_MOTOR_1, LOW);  // Run motor 
  digitalWrite(common::PIN_MOTOR_2, HIGH);  // Run motor 
  delay(500); // Run 500 ms to avoid current sensor noise

  unsigned long init_time = millis();
  while(fabs(I) < cg::CURRENT_THRESHOLD) // Still moving
  {
    I=cg::get_current(200); //Read current over 200 samples

    // If operation takes too long, there is probably an error. 
    // Stop the motor and exit
    if(millis() - init_time > cg::MAX_CLOSING_TIME_MILLIS)
      {
        digitalWrite(common::PIN_STBY, LOW); // Disable motor
        digitalWrite(common::PIN_MOTOR_1, LOW);  // Stop motor 
        digitalWrite(common::PIN_MOTOR_2, LOW);  // Stop motor 
        return ERROR::TIMEOUT;
      }
  }

  digitalWrite(common::PIN_STBY, LOW); // Disable motor
  digitalWrite(common::PIN_MOTOR_1, LOW);  // Stop motor 
  digitalWrite(common::PIN_MOTOR_2, LOW);  // Stop motor 
  return ERROR::OK;
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



void setup() 
{
  /******* Declaration of pin modes *******/

  pinMode(common::PIN_STBY,           OUTPUT);
  pinMode(common::PIN_MOTOR_1,        OUTPUT);
  pinMode(common::PIN_MOTOR_2,        OUTPUT);
  pinMode(common::PIN_MOTOR_PWM,      OUTPUT);
  pinMode(common::PIN_TOF_OUT,        OUTPUT);
  pinMode(common::PIN_GRIPPER_STATE,  OUTPUT);

  pinMode(common::PIN_OPERATION_CONTROL,  INPUT);
  pinMode(common::PIN_GRIPPER_TYPE,       INPUT);
  pinMode(common::PIN_GRIPPER_ACTION,     INPUT);
  pinMode(common::PIN_CURRENTSENSOR,      INPUT);

  DISTANCE_THRESHOLD_SIGNAL = false;

  /****************************************/

  // TOF activation
  Serial.begin(9600);
  TOF_AVAILABLE = lox.begin();

  // Check the gripper selected
  gripper_type = common::get_gripper_type();
  // For pneumatic gripper, no further actions required
  if(gripper_type != GRIPPER::CLASSIC){
    Serial.println("Initiation of routine.");
    return; 
  }
    

  // Open gripper to begin tasks
  ERROR open_output = cg::open_gripper();
  if(open_output == ERROR::OK){
    Serial.println("Gripper opened. Initiation of routine.");
    classic_gripper_state = cg::UR_ACTION::OPEN_GRIPPER;
    return;
  }
  if(open_output == ERROR::TIMEOUT){
    Serial.println("Gripper not fully opened. Initiation of routine with errors.");
    classic_gripper_state = cg::UR_ACTION::ERROR;
    return;
  }  

  // If no returns have happened, there are unhandled situations. Stop 
  Serial.println("Errors during initialization. Routine will not begin until reset.");
  while(true){}
}

void loop() 
{
  // Update the gripper selected
  gripper_type = common::get_gripper_type();
  // Debug info
  gripper_type == GRIPPER::CLASSIC ? Serial.println("[UR] Classic gripper selected") : Serial.println("[UR] Pneumatic gripper selected");

  // Update the state of the UC
  tof_mode = common::get_tof_configuration();
  // Debug info
  tof_mode == TOFMODE::DETECTING ? Serial.println("[UR] TOF configured to detect the pieces") : Serial.println("[UR] TOF configured to detect the L");

  // 1. Read tof and send signal to ur /////////////////////////////////////////////
  Serial.println("Reading TOF");
  ERROR tof_output = common::read_TOF(GRIPPER::CLASSIC, TOFMODE::POSITIONING);

  if(tof_output == ERROR::TOF_UNAVAILABLE)
    Serial.println("TOF sensor unavailable");
  if(tof_output == ERROR::OK)
    digitalWrite(common::PIN_TOF_OUT, DISTANCE_THRESHOLD_SIGNAL);

  // 2. Operate according to gripper type //////////////////////////////////////////
  // Only classic gripper needs further operations from the UC
  if(gripper_type != GRIPPER::CLASSIC)
    return;

  // 3. Get request from the UR ////////////////////////////////////////////////////
  cg::UR_ACTION ur_action = cg::get_action();
  switch(ur_action)
  {
    // Open gripper: check current state. 
    //    If gripper is open, do not do anything and continue
    //    If gripper is not open, open and update state
    //          State may be either open or error if failed 
    case cg::UR_ACTION::OPEN_GRIPPER:
    {
      Serial.println("[UR] Open gripper request");
      if(classic_gripper_state == cg::UR_ACTION::OPEN_GRIPPER)
      {
        Serial.println("Gripper already opened");
        break;
      }
      ERROR opening_output = cg::open_gripper();
      if(opening_output == ERROR::OK)
      {
        Serial.println("Gripper opened successfully");
        classic_gripper_state = cg::UR_ACTION::OPEN_GRIPPER;
      }
      else
      {
        Serial.println("Gripper opened with errors");
        classic_gripper_state = cg::UR_ACTION::ERROR;     
      }
      break;
    }

    // Close gripper: check current state. 
    //    If gripper is closed, do not do anything and continue
    //    If gripper is not closed, close and update state
    //          State may be either closed or error if failed 
    case cg::UR_ACTION::CLOSE_GRIPPER:
    {
      Serial.println("[UR] Close gripper request");
      if(classic_gripper_state == cg::UR_ACTION:: CLOSE_GRIPPER)
      {
        Serial.println("Gripper already closed");
        break;
      }
      ERROR closing_output = cg::close_gripper();
      if(closing_output == ERROR::OK)
      {
        Serial.println("Gripper closed successfully");
        classic_gripper_state = cg::UR_ACTION::CLOSE_GRIPPER;
      }
      else
      {
        Serial.println("Gripper opened with errors");
        classic_gripper_state = cg::UR_ACTION::ERROR;     
      }
      break;
    }
    case cg::UR_ACTION::ERROR:
      Serial.println("[UR] Error reading instruction");
  }

  // 4. Send updated information to the UR /////////////////////////////////////////
  if(classic_gripper_state != cg::UR_ACTION::ERROR)
    digitalWrite(common::PIN_GRIPPER_STATE, classic_gripper_state == cg::UR_ACTION::OPEN_GRIPPER);
}