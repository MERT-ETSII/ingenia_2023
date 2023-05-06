#include <Arduino.h>
#include <Wire.h>
#include<LiquidCrystal_I2C.h>
#include "Config.h"
#include "Pinout.h"
#if defined(ESP32)
  #include <WiFi.h>
  
#else
  #include <ESP8266WiFi.h>
#endif
#include<ModbusIP_ESP8266.h>
#include<Keypad.h>
#include<ezButton.h>
#include<LiquidCrystal_I2C.h>

extern IPAddress remote;



// ================================== Declaracion objetos y variables globales ==================

// display 
LiquidCrystal_I2C Lcd(ADDRESS_LCD, COLUMN_NUM, ROW_NUM); 

// modbus
ModbusIP mb;

// Teclado
Keypad Teclado = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// Botones
ezButton B_Llamar(PIN_BOTON_LLAMAR);
ezButton B_Enviar(PIN_BOTON_ENVIAR);
ezButton B_Rearmar(PIN_BOTON_REARME);
ezButton B_Stop(PIN_BOTON_STOP);

// Variable de estado
state Estado = S_INICIALIZANDO;

// Para hacer parpadear el led
unsigned long previousMillis = 0;
const long interval = 500;
int ledState = LOW;

// Para saber donde esta AGV
uint16_t pos_agv;

// Codigo de fallo y pieza
uint16_t buffer_fallo; // 0-
uint16_t buffer_superficie;
uint16_t buffer_pos;
char aux;

// Datos de las piezas
uint16_t datos_piezas[NUM_MAX_UTILLAJES] = 
{
0,
0,
0,
0,
0};

// ================================================= Setup ====================================
void setup()
{
  // Inicializamos pantalla
  Lcd.init();
  Lcd.backlight();
  Lcd.print("Inicializando...");


  
  // Inicializamos teclado
  Teclado.begin(makeKeymap(keys));

  // Inicializacion puerto serie
  Serial.begin(115200);

#ifndef DEBUG_BUTTONS

  // Configuracion conexion wifi
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi connected");
  Serial.println("IP Adress: ");
  Serial.println(WiFi.localIP());

  // COnfiguramos modbus como cliente
  mb.client();

  #endif

  // Configuramos como salidas los pines de los leds
  pinMode(PIN_LED_ENVIAR, OUTPUT);
  pinMode(PIN_LED_LLAMAR, OUTPUT);
  pinMode(PIN_LED_REARME, OUTPUT);

  Lcd.clear();
  Lcd.setCursor(0,0);
    Lcd.print("Inicializando...");
    Lcd.setCursor(0,1);
    Lcd.print("Listo");
  Estado = S0_IDLE;
  delay(2000);

  Lcd.clear();
  Lcd.print("[ 4.Tech ]");
  delay(1000);
  B_Llamar.loop();
  while(B_Llamar.getState())
  {
    B_Llamar.loop();
  }

  Lcd.clear();

}

// ================================================= Loop ======================================
void loop()
{

  // Librearias de los botones hacen sus cosas
  B_Enviar.loop();
  B_Llamar.loop();
  B_Rearmar.loop();
  B_Stop.loop();
#ifndef DEBUG_BUTTONS


  // Libreria modbus hace sus cosas
  mb.task();

  

  // Si estamos conectados
  if(mb.isConnected(remote))
  {
    // Estado 0
    if(Estado == S0_IDLE)
    {
      // Todas las luces apagadas
      digitalWrite(PIN_LED_ENVIAR, LOW);
      digitalWrite(PIN_LED_LLAMAR, LOW);
      digitalWrite(PIN_LED_REARME, LOW);

      // Display
      Lcd.clear();
      Lcd.setCursor(0,0);
      Lcd.print("Esperando");
      Lcd.setCursor(0,1);
      Lcd.print("llamada");
      

      if(!B_Llamar.getState())
      {
        // Cambiar el registro que se necesario para que venga el agv
        mb.writeCoil(remote,COIL_MISION_2, HIGH);

        // Cambiamos al siguiente estado
        Estado = S1_AGV_MOVIENDOSE_A_PUESTO;

      }


    }

    if(Estado == S1_AGV_MOVIENDOSE_A_PUESTO)
    {
      // Encendemos LED de llamada
      digitalWrite(PIN_LED_ENVIAR, LOW);
      digitalWrite(PIN_LED_LLAMAR, HIGH);
      digitalWrite(PIN_LED_REARME, LOW);

      // Leemos posicion del agv
      mb.readHreg(remote, REG_POS_AGV, &pos_agv);

      // Comprobamos si ha llegado al puesto de operario
      if(pos_agv == EN_OPERARIO)
      {
        Estado = S2_ESPERANDO_CODIGO_FALLO;
        buffer_fallo = -1; // Para poder comprobar si se ha introducido otro
      }
    }

    if(Estado == S2_ESPERANDO_CODIGO_FALLO)
    {
      // LED llamar parpadea
      if(millis() - previousMillis >= interval)
      {
        ledState != ledState;
        previousMillis = millis();
      }
      digitalWrite(PIN_LED_LLAMAR, ledState);

      // Lcd
      Lcd.clear();
      Lcd.setCursor(0,0);
      
      Lcd.print("Introduce codigo de fallo");

      // Leemos el teclado
      aux = Teclado.getKey();

      // Si aux es un num
      if (aux >= '0' && aux <= '9')
      {
      int aux_ = aux - '0';
      buffer_fallo = (uint16_t) aux_;
      }

      // Lo mostramos en la segunda linea de la pantalla
      Lcd.setCursor(0,1);
      Lcd.print(buffer_fallo);
      
      

      // Cambio de estado ssi se ha pulsado aceptar (a) y se ha introducido un fallo valido (fallo != -1)
      if(buffer_fallo != -1 && aux == ACEPTAR)
      {
        Estado = S4_ESPERANDO_POS_PIEZA;
        buffer_pos = -1; // Para poder comprobar si se ha introducido
      }
    }

    if(Estado == S3_ESPERANDO_CODIGO_FALLO_SUPERFICIE)
    {
      // LED llamar parpadea
      if(millis() - previousMillis >= interval)
      {
        ledState != ledState;
        previousMillis = millis();
      }
      digitalWrite(PIN_LED_LLAMAR, ledState);

      // Lcd
      Lcd.clear();
      Lcd.setCursor(0,0);
      Lcd.print("Introducir superficie del fallo");

      // Leemos el teclado
      aux = Teclado.getKey();

      // Si aux es un num lo anadimos al final del codigo del fallo
      if (aux >= '0' && aux <= '9')
      {
      int aux_ = aux - '0';
      buffer_superficie = (uint16_t) aux_;
      }

      // Lo mostramos en la segunda linea de la pantalla
      Lcd.setCursor(0,1);
      Lcd.print(buffer_superficie);
      
      

      // Cambio de estado ssi se ha pulsado aceptar (a) y se ha introducido un fallo valido (fallo != -1)
      if(buffer_superficie != -1 && aux == ACEPTAR)
      {
        Estado = S4_ESPERANDO_POS_PIEZA;
        buffer_pos = -1; // Para poder comprobar si se ha introducido
      }
    }

    if(Estado == S4_ESPERANDO_POS_PIEZA)
    {
      // LED llamar parpadea
      if(millis() - previousMillis >= interval)
      {
        ledState != ledState;
        previousMillis = millis();
      }
      digitalWrite(PIN_LED_LLAMAR, ledState);
      digitalWrite(PIN_LED_ENVIAR, LOW);
      digitalWrite(PIN_LED_ENVIAR, LOW);

      // Lcd
      Lcd.setCursor(0,0);
      Lcd.clear();
      Lcd.print("Introduce posicion");

      // Leemos el teclado
      aux = Teclado.getKey();

      // Si aux es un num lo almacenamos en el buffer correspondiente
      if (aux >= '0' && aux <= '4')
      {
      int aux_ = aux - '0';
      buffer_pos = (uint16_t) aux_;
      }

      // Lo mostramos en la segunda linea de la pantalla
      Lcd.setCursor(0,1);
      String str = "Posicion [" + String(buffer_fallo-1) + "]";
      Lcd.print(str);

      // Si se pulsa atras volvemos al estado de esperar codigo de fallo
      if(aux == ATRAS)
      {
        Estado = S2_ESPERANDO_CODIGO_FALLO;
      }

      // Si se ha introducido correctamente y se pulsa aceptar pasamos a siguiente pantalla
      if(aux == ACEPTAR && buffer_pos != -1)
      {
        Estado == S5_ESPERANDO_ENVIAR;

        // Almacenamos que fallo hay en que posicion (0 si no hay pieza)
        datos_piezas[buffer_pos] = buffer_fallo * 10 + buffer_superficie;
      }
    }

    if(Estado == S6_ANADIR_OTRA_PIEZA)
    {
      // LED llamar parpadea
      if(millis() - previousMillis >= interval)
      {
        ledState != ledState;
        previousMillis = millis();
      }
      digitalWrite(PIN_LED_LLAMAR, ledState);
      digitalWrite(PIN_LED_ENVIAR, LOW);
      digitalWrite(PIN_LED_ENVIAR, LOW);

      // Lcd
      Lcd.setCursor(0,0);
      Lcd.print("Quieres anadir o modificar algo?");
      Lcd.setCursor(0,1);
      Lcd.print("Si (A) No (B)");

      // Leemos el teclado
      aux = Teclado.getKey();

      // Si se pulsa A, volvemos al estado de esperar tipo de fallo
      if(aux == ACEPTAR)
      {
        Estado = S2_ESPERANDO_CODIGO_FALLO;
      }
      if(aux == 'B')
      {
        Estado = S5_ESPERANDO_ENVIAR;
      }
      
      
    }

    if(Estado == S5_ESPERANDO_ENVIAR)
    {
      // LED enviar parpadea
      if(millis() - previousMillis >= interval)
      {
        ledState != ledState;
        previousMillis = millis();
      }
      digitalWrite(PIN_LED_LLAMAR, LOW);
      digitalWrite(PIN_LED_ENVIAR, ledState);
      digitalWrite(PIN_LED_ENVIAR, LOW);

      // Lcd
      Lcd.clear();
      Lcd.setCursor(0,0);
      Lcd.print("Listo para enviar");

      // Mostramos las piezas en la pantalla
      String str = "";
      for(int i = 0; i < NUM_MAX_UTILLAJES; i++)
      {
        str = str + "(" + String(i) + ", " + String(datos_piezas[i]) + ") ";
      }
      Lcd.setCursor(0,1);
      Lcd.print(str);

      // Leemos teclado
      aux = Teclado.getKey();

      // Si pulsamos atras
      if(aux == ATRAS)
      {
        Estado = S6_ANADIR_OTRA_PIEZA;
      }

      // Si pulsamos enviar
      if(!B_Enviar.getState())
      {
        // Guardamos los datos de las piezas en los registros
        for(uint8_t i = 0; i < NUM_MAX_UTILLAJES; i++)
        {
          mb.writeHreg(remote, REG_PIEZAS[i], datos_piezas[i]);
        }

        // Habria que activar el trigger para que vaya hacia el robot
        mb.writeCoil(remote, COIL_MISION_2, HIGH);

        Estado = S7_PULSADO_ENVIAR;
        previousMillis = millis();
      }
      
 
    }

    if(Estado == S7_PULSADO_ENVIAR)
    {
      // LED enviar encendido
      // LED llamar parpadea
      digitalWrite(PIN_LED_LLAMAR, LOW);
      digitalWrite(PIN_LED_ENVIAR, HIGH);
      digitalWrite(PIN_LED_ENVIAR, LOW);

      // Nos quedamos en este estado un cierto tiempo antes de pasar a IDLE
      // Lcd
      Lcd.clear();
      Lcd.setCursor(0,0);
      Lcd.print("Enviado!");
      
      if(millis() - previousMillis >= T_S6)
      {
        // Ponemos todos los buffers y datos a default
        buffer_fallo = -1;
        buffer_pos = -1;  
        for(uint8_t i = 0; i < NUM_MAX_UTILLAJES; i++)
        {
          datos_piezas[i] = 0;
        }

        // Volvemos al estado inicial
        Estado = S0_IDLE;
      }
    }

    if(Estado == S8_REARME)
    {

    }

    if(Estado == S9_STOP)
    {

    }


  }
  else
  {

    Lcd.setCursor(0,0);
    Lcd.print("ERROR:");
    Lcd.setCursor(0,1);
    Lcd.print("No remote");
    mb.connect(remote);
  }
#endif

#ifdef DEBUG_BUTTONS

  // Boton enviar
  if(!B_Enviar.getState()) 
  {
     
    Lcd.print("ENVIAR");
    Serial.println("ENVIAR pulsado");
    digitalWrite(PIN_LED_ENVIAR, HIGH);
  }
  else
  {
    Lcd.clear();
    digitalWrite(PIN_LED_ENVIAR, LOW);
  }

  // Boton llamar
  if(!B_Llamar.getState()) 
  {
    
    Lcd.print("Llamar");
    Serial.println("Llamar pulsado");
    digitalWrite(PIN_LED_LLAMAR, HIGH);
  }
  else
  {
    Lcd.clear();
    digitalWrite(PIN_LED_LLAMAR, LOW);
  }

  // Boton rearme
  if(!B_Rearmar.getState()) 
  {
    
    Lcd.print("REARMAR");
    Serial.println("REARMAR pulsado");
    digitalWrite(PIN_LED_REARME, HIGH);
  }
  else
  {
    Lcd.clear();
    digitalWrite(PIN_LED_REARME, LOW);
  }
  
  // Boton stop
  if(!B_Stop.getState()) 
  {
    Serial.println("REARMAR pulsado");
    digitalWrite(PIN_LED_REARME, HIGH);
    digitalWrite(PIN_LED_LLAMAR, HIGH);
  }
  else
  {
    digitalWrite(PIN_LED_REARME, LOW);
    digitalWrite(PIN_LED_LLAMAR, LOW);
  }

 
    char key = Teclado.getKey();
    Lcd.clear();
    Lcd.print(key);
  
  

#endif
}