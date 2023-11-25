#include <Arduino.h>
//wart serial ]




// Definiciones de pines para el sensor de proximidad ultrasónico
#define TRIGGER_PIN_1  3  // Pin de trigger del primer sensor ultrasónico
#define ECHO_PIN_1     2  // Pin de echo del primer sensor ultrasónico

#define TRIGGER_PIN_2  7  // Pin de trigger del segundo sensor ultrasónico
#define ECHO_PIN_2     6  // Pin de echo del segundo sensor ultrasónico

//definimoss otro sensor ultra sonico
#define TRIGGER_PIN_3  11  // Pin de trigger del segundo sensor ultrasónico
#define ECHO_PIN_3     10  // Pin de echo del segundo sensor ultrasónico

#define LedPin_1  13 
#define LedPin_2  12


void setup() {

  Serial.begin(115200);
// Configuración del sensor de proximidad
  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIGGER_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
    pinMode(TRIGGER_PIN_3, OUTPUT);
    pinMode(ECHO_PIN_3, INPUT);

  // Configuración de los leds
  pinMode(LedPin_1, OUTPUT);
  pinMode(LedPin_2, OUTPUT);
  // Valores que mandamos
  pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(4, OUTPUT);


}


void loop() {
  // Detección de proximidad
  long duration_1, distance_1;
  long duration_2, distance_2;
  // Medir la distancia del primer sensor
  digitalWrite(TRIGGER_PIN_1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN_1, LOW);
  duration_1 = pulseIn(ECHO_PIN_1, HIGH);
  distance_1 = duration_1 * 0.034 / 2;

  // Medir la distancia del segundo sensor
  digitalWrite(TRIGGER_PIN_2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN_2, LOW);
  duration_2 = pulseIn(ECHO_PIN_2, HIGH);
  distance_2 = duration_2 * 0.034 / 2;

 

    // Imprimir la distancia en el monitor serial
    Serial.print("Distancia 1: ");
    Serial.print(distance_1);
    Serial.print(" cm, Distancia 2: ");
    Serial.print(distance_2);
    Serial.println(" cm");
  if(distance_1 < 20){
    digitalWrite(LedPin_1, HIGH);
    digitalWrite(8, HIGH);
    //deten el carro
  }

  else{
    digitalWrite(LedPin_1,LOW);
    digitalWrite(8, LOW);
  }

  if(distance_2 < 20){
    digitalWrite(LedPin_2, HIGH);
    digitalWrite(9, HIGH);
    //need to send esp32 a signal to stop the car by the wart serial
  }
  else{
    digitalWrite(LedPin_2, LOW);
    digitalWrite(9, LOW);
  }


    delay(50);

}