/**
 * Code that will be running in the control unit of the UR
 * 
 * It needs to control the motor gripper and the pneumatic one under commands from the robotic arm. 
 * It also needs to be able to read and process distances from a ToF sensor, and send and receive data from the UR.
 * 
*/

#define DEBUG_UR 0
#define DEBUG_UC 0

#include <Arduino.h>
#include <math.h>
#include "pinout.h"
#include "classic_gripper.h"

using namespace type;

/******* Generic initializations  *******/

// State variables
GRIPPER gripper_type = GRIPPER::CLASSIC;  // Selected gripper
tof::TOFMODE tof_mode = tof::TOFMODE::POSITIONING;  // TOF configuration

cg::UR_ACTION classic_gripper_state = cg::UR_ACTION::OPEN_GRIPPER;  // To keep track of the gripper position

/****************************************/


void setup() 
{
  /******* Declaration of pin modes *******/

  pinMode(PIN_STBY,           OUTPUT);
  pinMode(PIN_MOTOR_1,        OUTPUT);
  pinMode(PIN_MOTOR_2,        OUTPUT);
  pinMode(PIN_MOTOR_PWM,      OUTPUT);
  pinMode(PIN_TOF_OUT,        OUTPUT);
  pinMode(PIN_GRIPPER_STATE,  OUTPUT);

  pinMode(PIN_OPERATION_CONTROL,  INPUT);
  pinMode(PIN_GRIPPER_TYPE,       INPUT);
  pinMode(PIN_GRIPPER_ACTION,     INPUT);
  pinMode(PIN_CURRENTSENSOR,      INPUT);

  

  /****************************************/
  
  Serial.begin(9600);
  
  tof::DISTANCE_THRESHOLD_SIGNAL = false;
  // TOF activation
  tof::TOF_AVAILABLE = tof::lox.begin();
  if(!tof::TOF_AVAILABLE) Serial.println("TOF sensor unavailable during setup");

  // Check the gripper selected
  gripper_type = get_gripper_type();

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
  // delay(2000);
  
  // Update the gripper selected
  gripper_type = get_gripper_type();
  // Debug info
  if(DEBUG_UR)
    gripper_type == GRIPPER::CLASSIC ? Serial.println("[UR] Classic gripper selected") : Serial.println("[UR] Pneumatic gripper selected");

  // Update the state of the UC
  tof_mode = tof::get_tof_configuration();
  // Debug info
  if(DEBUG_UR)
    tof_mode == tof::TOFMODE::DETECTING ? Serial.println("[UR] TOF configured to detect the pieces") : Serial.println("[UR] TOF configured to detect the L");


  // 1. Read tof and send signal to ur /////////////////////////////////////////////
  if(DEBUG_UC)
    Serial.println("Reading TOF");
  ERROR tof_output = tof::read_TOF(gripper_type, tof_mode);

  if(tof_output == ERROR::TOF_UNAVAILABLE)
    Serial.println("TOF sensor unavailable");
  if(tof_output == ERROR::OK)
    digitalWrite(PIN_TOF_OUT, tof::DISTANCE_THRESHOLD_SIGNAL);


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
      if(DEBUG_UR)
        Serial.println("[UR] Open gripper request");
      if(classic_gripper_state == cg::UR_ACTION::OPEN_GRIPPER)
      {
        if(DEBUG_UC)
          Serial.println("Gripper already opened");
        break;
      }
      ERROR opening_output = cg::open_gripper();
      if(opening_output == ERROR::OK)
      {
        if(DEBUG_UC)
          Serial.println("Gripper opened successfully");
        classic_gripper_state = cg::UR_ACTION::OPEN_GRIPPER;
      }
      else
      {
        if(DEBUG_UC)
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
      if(DEBUG_UR)
        Serial.println("[UR] Close gripper request");
      if(classic_gripper_state == cg::UR_ACTION:: CLOSE_GRIPPER)
      {
        if(DEBUG_UR)
          Serial.println("Gripper already closed");
        break;
      }
      ERROR closing_output = cg::close_gripper();
      if(closing_output == ERROR::OK)
      {
        if(DEBUG_UR)
          Serial.println("Gripper closed successfully");
        classic_gripper_state = cg::UR_ACTION::CLOSE_GRIPPER;
      }
      else
      {
        if(DEBUG_UR)
          Serial.println("Gripper opened with errors");
        classic_gripper_state = cg::UR_ACTION::ERROR;     
      }
      break;
    }
    case cg::UR_ACTION::ERROR:
      if(DEBUG_UR)
        Serial.println("[UR] Error reading instruction");
  }


  // 4. Send updated information to the UR /////////////////////////////////////////
  if(classic_gripper_state != cg::UR_ACTION::ERROR)
    digitalWrite(PIN_GRIPPER_STATE, classic_gripper_state == cg::UR_ACTION::OPEN_GRIPPER);
}