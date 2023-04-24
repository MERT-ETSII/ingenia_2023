#include <Arduino.h>

#if defined(ESP32)
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <ModbusIP_ESP8266.h>
#include <Adafruit_VL53L0X.h>