/// This program sends a signal to the motor driver until the current sensor detects
/// a spike (the motor has impeded movement) and then it stops

#include <Arduino.h>

int MOTOR = 4;
int CURRENTSENSOR = 0;

// float Sensibilidad=0.185; //sensibilidad en Voltios/Amperio para sensor de 5A
float Sensibilidad=0.3; //sensibilidad en Voltios/Amperio para sensor de 5A

float get_current(int n_muestras)
{
  // Corriente en amperios: (lectura analógica - 553.2731257)/31.17464354
  // Asumiendo que la lectura analógica de (0-1023)
  // Fuente: confía
  float voltajeSensor;
  float corriente=0;
  for(int i=0;i<n_muestras;i++)
  {
    // voltajeSensor = analogRead(CURRENTSENSOR) * (3 / 1023.0);////lectura del sensor
    // corriente=corriente+(voltajeSensor-1.5)/Sensibilidad; //Ecuación  para obtener la corriente
    voltajeSensor = (analogRead(CURRENTSENSOR) - 553.2731257)/31.17464354;
    corriente += voltajeSensor;
  }
  corriente=corriente/n_muestras;
  return(corriente);
}


void setup() {
  pinMode(MOTOR, OUTPUT);
  Serial.begin(9600);
  Serial.println("############## BEGIN ##############");
}
void stop(){
  while (true){

  }
}

/// @brief When current is over or below a threshold, the code goes to stop()
void loop() {
  
  float I=get_current(200);//obtenemos la corriente promedio de 200 muestras 
  if(I < -0.4 || I > 0.4){
    Serial.print("STOP: ");
    Serial.println(I);
    digitalWrite(MOTOR,LOW);
  }
  else{
      Serial.print("Corriente: ");
      Serial.println(I,3); 
      digitalWrite(MOTOR,HIGH);
  }
     

  delay(500);
}