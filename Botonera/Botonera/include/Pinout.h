#ifndef PINOUT_H
#define PINOUT_H
#include<Arduino.h>
#include"Config.h"

/* Fichero para definicion de pines y direcciones de memoria i2c*/


// Pantalla LCD
const uint8_t PIN_LCD_BACKLIGHT = 3;
const uint8_t ADDRESS_LCD = 0x27;

// Botones
const uint8_t PIN_BOTON_LLAMAR = 27;
const uint8_t PIN_BOTON_ENVIAR = 25;
const uint8_t PIN_BOTON_REARME = 26;
const uint8_t PIN_BOTON_STOP = 14;

// Leds 
const uint8_t PIN_LED_ENVIAR = 32;
const uint8_t PIN_LED_LLAMAR = 33;
const uint8_t PIN_LED_REARME = 35;



// Teclado
// https://arduinogetstarted.com/tutorials/arduino-keypad
byte pin_rows[ROW_NUM] = {19,18,5,17}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {16,4,2,15}; //connect to the column pinouts of the keypad

#endif