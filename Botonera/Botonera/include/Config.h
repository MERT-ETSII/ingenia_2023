#ifndef CONFIG_H
#define CONFIG_H
#include<Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
  
#else
  #include <ESP8266WiFi.h>
#endif
#include<ModbusIP_ESP8266.h>



// ==================================== Compilacion condicional =====================


// Descomentar para comprobar funcionamiento correcto de los botones. 
#define NO_MODBUS
//#define DEBUG_BUTTONS


/* Fichero para los distintos parametros de configuraicon*/

// ==================================== Maquina de estados ==========================

// Estados posibles
enum state
{
  S_INICIALIZANDO,
  S0_IDLE,
  S1_AGV_MOVIENDOSE_A_PUESTO,
  S2_ESPERANDO_CODIGO_FALLO,
  S3_ESPERANDO_CODIGO_FALLO_SUPERFICIE,
  S4_ESPERANDO_POS_PIEZA,
  S5_ESPERANDO_ENVIAR,
  S6_ANADIR_OTRA_PIEZA,
  S7_PULSADO_ENVIAR,
  S8_REARME,
  S9_STOP
};

// Tiempo en ms durante el cual se permanece en el estado S6
const long int T_S6 = 5000;

// Numero de piezas que puede llevar el AGV
#define NUM_MAX_UTILLAJES 5

// ==================================== MODBUS ======================================

// --- Conexion wifi ---

// Wifi password and SSID
const char* ssid = "RG";//"SMARTMAI";
const char* password = "tengo2perros";//"unidad_fabri";

// Direccion del servidor
IPAddress remote(192, 168, 0, 29);  // Direccion del servidor

// --- Puertos ---

// Registro de posicion del AGV: Contiene informacion de donde esta situado el AGV
// Ejemplo:
// 0 -> En puesto de carga
// 1 -> En puesto operario
// 2 -> En puesto UR
// 3 -> en movimiento

const uint16_t REG_POS_AGV = 1000;
const uint16_t EN_MOVIMIENTO = 3;
const uint16_t EN_CARGA = 0;
const uint16_t EN_UR = 1;
const uint16_t EN_OPERARIO = 2;

// Registros para info de las piezas
const uint16_t REG_PIEZAS[NUM_MAX_UTILLAJES] = 
{
  1199, // Pos 0      ======= AGV =========               
  1200, // Pos 1      -------------------
  1201, // Pos 2      |  [0]        [1]  |
  1202, // Pos 3      |                  |
  1203  // Pos 4      |                  |
};      //            |       [2]        |   
                   // |                  |
                   // |                  |
                   // |  [3]        [4]  |
                   // -------------------



// Trigger mision para que vaya a puesto
const uint16_t COIL_MISION_1 = 100;
const uint16_t COIL_MISION_2 = 101;




// ==================================== TECLADO ======================================
// Disposicion del teclado
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //three columns

const char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','a'},
  {'4','5','6','b'},
  {'7','8','9','c'},
  {'*','0','#','d'}
};

const char ACEPTAR = 'a';
const char ATRAS = 'b';

/*
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino
lib_deps = 
    fmalpartida/LiquidCrystal@^1.5.0
    emelianov/modbus-esp8266@^4.1.0
*/

// ==================================== TIPOS DE FALLO ======================================

/*
Error tolerancia dimensional 1
Error tolerancia geometrica 2
Error acabado 3
*/

#define CFALLO_MAX 9;
#define CFALLO_MIN 0;



#endif