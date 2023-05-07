#ifndef PINOUT_H
#define PINOUT_H
#include<Arduino.h>
#include"Config.h"

/* Fichero para definicion de pines y direcciones de memoria i2c*/


// Pantalla LCD
const uint8_t PIN_LCD_BACKLIGHT = 3;
const uint8_t ADDRESS_LCD = 0x27;

// Botones
const uint8_t PIN_BOTON_LLAMAR = 14;
const uint8_t PIN_BOTON_ENVIAR = 26;
const uint8_t PIN_BOTON_REARME = 27;
const uint8_t PIN_BOTON_STOP = 34;

// Leds 
const uint8_t PIN_LED_ENVIAR = 33;
const uint8_t PIN_LED_LLAMAR = 32;
const uint8_t PIN_LED_REARME = 25;



// Teclado//17
// https://arduinogetstarted.com/tutorials/arduino-keypad
byte pin_rows[ROW_NUM] = {17,5,18,19}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {15,2,4,16}; //connect to the column pinouts of the keypad

#endif