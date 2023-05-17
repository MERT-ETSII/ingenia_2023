    #include <Arduino.h>
    
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
    /// @brief  TRUE: Gripper OPENED; FALSE: Gripper CLOSED
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