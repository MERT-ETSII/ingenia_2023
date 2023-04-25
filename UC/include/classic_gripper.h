enum class Error
{
    OK = 0,
    TIMEOUT = -1
};

namespace cg
{
    unsigned long MAX_CLOSING_TIME_MILLIS = 3000;

    /// @brief Analog input from current sensor
    const uint8_t PIN_CURRENTSENSOR = 0;

    /// @brief OFF: CLASSIC; ON: PNEUMATIC
    const uint8_t PIN_TOF_IN = 1;

    /// @brief GPIO2 (D4) - OFF: Further than threshold; ON: Closer than threshold
    const uint8_t PIN_TOF_OUT = 2;

    /// @brief OFF: Threshold for positioning of the gripper ON: Threshold for detecting objects
    const uint8_t PIN_STATE_POSITION_DETECTION = 3;

    /// @brief GPIO4 (D2) - OFF: CLASSIC; ON: PNEUMATIC
    const uint8_t PIN_TOF_I2C1 = 4;

    /// @brief GPIO5 (D1) - OFF: CLASSIC; ON: PNEUMATIC
    const uint8_t PIN_TOF_I2C2 = 5;

    /// @brief  Digital output to the enable of the motor driver
    const uint8_t PIN_MOTOR = 6;

    /// @brief Maximum peak current (in Amperes) to the motor until stop signal is sent
    float CURRENT_THRESHOLD = 0.4;

    //@brief Function to obtain current averaged over n samples
    //@param[in] n_samples: number of iterations to use for current filter
    float get_current(int n_samples);

    
    /*
    * Function that performs the closing action. 
    *
    * This implies sending a signal to the motor to begin movement, 
    * read the current sensor until it reaches certain threshold 
    * (overcurrent means that the motor is blocked, so the object is picked) 
    * and exiting. 
    * If the action takes too long, an error is returned.
    */
    Error close_gripper();
}