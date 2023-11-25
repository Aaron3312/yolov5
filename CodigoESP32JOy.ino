#include <Arduino.h>

// Definir los pines del joystick
#define JOYSTICK_X A0
#define JOYSTICK_Y A1

void setup() {
    Serial.begin(115200);
}

void loop() {
    // Leer los valores del joystick
    int joystickX = analogRead(JOYSTICK_X);
    int joystickY = analogRead(JOYSTICK_Y);

    // Enviar comandos al ESP32 del brazo robótico y el carro
    if (joystickY < 100) {
        Serial.write(CMD_FORWARD);
    } else if (joystickY > 900) {
        Serial.write(CMD_BACKWARD);
    } else {
        Serial.write(CMD_STOP);
    }

    if (joystickX < 100) {
        Serial.write(CMD_LEFT);
    } else if (joystickX > 900) {
        Serial.write(CMD_RIGHT);
    } else {
        Serial.write(CMD_STOP);
    }

    delay(100); // Puedes ajustar este valor según sea necesario
}
