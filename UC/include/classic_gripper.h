enum class Error
{
    OK = 0,
    TIMEOUT = -1
};

namespace cg
{
    unsigned long MAX_CLOSING_TIME_MILLIS = 3000;
    int PIN_MOTOR = 4;
    int PIN_CURRENTSENSOR = 0;

    int CURRENT_THRESHOLD = 0.4;

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