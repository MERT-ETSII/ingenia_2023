#ifndef PINOUT_H
#define PINOUT_H
#include<Arduino.h>
#include"Config.h"

/* Fichero para definicion de pines y direcciones de memoria i2c*/


// Pantalla LCD
const uint8_t PIN_LCD_BACKLIGHT = 3;
const uint8_t ADDRESS_LCD = 0x38;

// Botones
const uint8_t PIN_BOTON_LLAMAR = 2;
const uint8_t PIN_BOTON_ENVIAR = 4;
const uint8_t PIN_BOTON_REARME = 5;
const uint8_t PIN_BOTON_STOP = 6;

// Leds 
const uint8_t PIN_LED_ENVIAR = 7;
const uint8_t PIN_LED_LLAMAR = 8;
const uint8_t PIN_LED_REARME = 12;

// Teclado
// https://arduinogetstarted.com/tutorials/arduino-keypad
byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 9}; //connect to the column pinouts of the keypad

#endif