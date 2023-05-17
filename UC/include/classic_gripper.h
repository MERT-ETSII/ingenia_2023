namespace cg
{
    unsigned long MAX_CLOSING_TIME_MILLIS = 3000;

    /// @brief Maximum peak current (in Amperes) to the motor until stop signal is sent
    float CURRENT_THRESHOLD = 0.4;

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

/*
    enum class GRIPPER_STATE
    {
        ERROR = -1,
        OPENED = 0,
        CLOSED =1,
    }
*/

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