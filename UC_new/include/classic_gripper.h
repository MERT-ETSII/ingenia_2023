#ifndef CLASSIC_GRIPPER_H
#define CLASSIC_GRIPPER_H

#include <Arduino.h>
#include "pinout.h"
#include "common.h"

namespace cg
{
    /// @brief Maximum peak current (in Amperes) to the motor until stop signal is sent
    extern float CURRENT_THRESHOLD;

    /// @brief Maximun closing time of the classic gripper
    extern unsigned long MAX_CLOSING_TIME_MILLIS;

    /// @brief Closing time of the classic gripper with no sensor reading
    extern unsigned int MIN_TIME_MILLIS;

    /// @brief Motor speed value
    extern unsigned int PWM_VALUE;

    /// @brief Function to obtain current averaged over n samples
    /// @param[in] n_samples: number of iterations to use for current filter
    float get_current(int n_samples);

    /// @brief Possible actions requested from the UR
    enum class UR_ACTION
    {
        ERROR = -1,
        OPEN_GRIPPER = 0,
        CLOSE_GRIPPER = 1
    };


    /// @brief Function that performs the closing action.
    /// This implies sending a signal to the motor to begin movement, 
    /// read the current sensor until it reaches certain threshold 
    /// (overcurrent means that the motor is blocked, so the object is picked) 
    /// and exiting. 
    /// If the action takes too long, an error is returned.
    /// @return 
    ERROR close_gripper();

    /// @brief Function that performs the opening action.
    /// This implies sending a signal to the motor to begin movement, 
    /// read the current sensor until it reaches certain threshold 
    /// (overcurrent means that the motor is blocked, gripper is fully open) 
    /// and exiting. 
    /// If the action takes too long, an error is returned.
    /// @return 
    ERROR open_gripper();

    /// @brief Function to read the request from the UR
    /// @return The action requested
    UR_ACTION get_action();
}

#endif