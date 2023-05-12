#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

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
    /// @brief Reserved for communications
    const uint8_t PIN_TX = 0;
    /// @brief Reserved for communications
    const uint8_t PIN_RX = 1;

    /// @brief Enable for the motor drive
    const uint8_t PIN_STBY = 2;

    /// @brief State of the first terminal of the motor
    const uint8_t PIN_MOTOR_1 = 3;

    /// @brief State of the second terminal of the motor
    const uint8_t PIN_MOTOR_2 = 4;

    /// @brief Pin to regulate power output to the motor
    const uint8_t PIN_MOTOR_PWM = 5;

    /// @brief  TRUE: Detection (distance under threshold); FALSE: No detection
    const uint8_t PIN_TOF_OUT = 6;

    /// @brief  TRUE: Gripper OPEN; FALSE: Gripper CLOSED
    const uint8_t PIN_GRIPPER_STATE = 7;

    /// @brief  TRUE: Distance to the piece; FALSE: Distance to the L
    const uint8_t PIN_OPERATION_CONTROL = 8;

    /// @brief  TRUE: pneumatic; FALSE: classic
    const uint8_t PIN_GRIPPER_TYPE = 9;

    /// @brief  TRUE: open gripper; FALSE: close gripper
    const uint8_t PIN_GRIPPER_ACTION = 10;

    /// @brief Analog input from the current sensor
    const uint8_t PIN_CURRENTSENSOR = A0;

    /// @brief For I2C communications with the TOF sensor
    const uint8_t PIN_TOF_SDA = A4;

    /// @brief For I2C communications with the TOF sensor
    const uint8_t PIN_TOF_SCL = A5;


    /// @brief Function to read the distance requests from the UR
    /// @return POSITIONING, for input 0 or DETECTING, for input 1
    TOFMODE get_tof_configuration();

    /// @brief Function that reads the value from the TOF sensor and sends a digital signal 
    /// wether the distance falls under certain threshold or not.
    /// @return 0 for no detection; 1 for detection
    Error read_TOF(GRIPPER gripper_type, TOFMODE tof_mode);

    /// @brief Get the gripper type read from UR
    /// @return CLASSIC, for input 0 or PNEUMATIC, for input 1
    GRIPPER get_gripper_type();
}
