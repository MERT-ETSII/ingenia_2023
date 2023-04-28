#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

/// @brief Defines the UC state
enum class UC_MODE
{
    ERROR = -1,
    IDLE = 0,
    SEND_TOF_SIGNAL = 1,
    OPEN_GRIPPER = 2,
    CLOSE_GRIPPER = 3
};

/// @brief Error handling enum
enum class Error
{
    OK = 0,
    NO_INSTRUCTION = -1,
    TIMEOUT = -2,
    TOF_UNAVAILABLE = -3
};

/// @brief Possible configurations for the TOF sensor
enum  class TOFMODE
{
    POSITIONING = 0,
    DETECTING = 1
};

/// @brief Available grippers
enum class GRIPPER
{
    CLASSIC = 0,
    PNEUMATIC = 1
};

namespace common
{
    /// @brief Analog input from current sensor
    const uint8_t PIN_CURRENTSENSOR = 2;

    /// @brief OFF: CLASSIC; ON: PNEUMATIC
    const uint8_t PIN_TOF_IN = 3;

    /// @brief GPIO2 (D4) - OFF: Further than threshold; ON: Closer than threshold
    const uint8_t PIN_TOF_OUT = 4;

    /// @brief OFF: Threshold for positioning of the gripper ON: Threshold for detecting objects
    const uint8_t PIN_STATE_POSITION_DETECTION = 5;

    /// @brief  Digital output to the enable of the motor driver
    const uint8_t PIN_MOTOR = 6;

    /// @brief GPIO4 (D2) - OFF: CLASSIC; ON: PNEUMATIC
    const uint8_t PIN_TOF_SDA = A4;

    /// @brief GPIO5 (D1) - OFF: CLASSIC; ON: PNEUMATIC
    const uint8_t PIN_TOF_SCL = A5;

    UC_MODE read_ur_task();

    /// @brief Function that reads the value from the TOF sensor and sends a digital signal 
    /// wether the distance falls under certain threshold or not.
    /// @return 
    Error read_TOF(GRIPPER gripper_type, TOFMODE tof_mode);
}
