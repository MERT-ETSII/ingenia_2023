#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include "pinout.h"

/// @brief Error handling enum
enum class ERROR
{
    OK = 0,
    NO_INSTRUCTION = -1,
    TIMEOUT = -2,
    TOF_UNAVAILABLE = -3
};

namespace type
{
    /// @brief Available grippers
    enum class GRIPPER
    {
        CLASSIC = 0,
        PNEUMATIC = 1
    };

    /// @brief Get the gripper type read from UR
    /// @return CLASSIC, for input 0 or PNEUMATIC, for input 1
    GRIPPER get_gripper_type();
}

namespace tof
{
    // TOF variables
    Adafruit_VL53L0X lox = Adafruit_VL53L0X(); // TOF driver
    VL53L0X_RangingMeasurementData_t measure;  // TOF measured distance
    bool DISTANCE_THRESHOLD_SIGNAL;
    bool TOF_AVAILABLE;

    /// @brief Possible configurations for the TOF sensor
    enum class TOFMODE
    {
        POSITIONING = 0,
        DETECTING = 1
    };
    
    /// @brief Function to read the distance requests from the UR
    /// @return POSITIONING, for input 0 or DETECTING, for input 1
    TOFMODE get_tof_configuration();

    /// @brief Function that reads the value from the TOF sensor and sends a digital signal 
    /// wether the distance falls under certain threshold or not.
    /// @return 0 for no detection; 1 for detection
    ERROR read_TOF(tof::GRIPPER tof::gripper_type, TOFMODE tof_mode);
}


