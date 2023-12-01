#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <ESP32Servo.h>
#include <iostream>
#include <sstream>

// Definiciones de pines usados para los motores
#define PIN_MOTOR_R_FWD 5
#define PIN_MOTOR_R_BWD 18
#define PIN_MOTOR_L_FWD 19
#define PIN_MOTOR_L_BWD 21

// Definiciones de pines para el sensor de proximidad ultrasónico
#define TRIGGER_PIN 4
#define ECHO_PIN 2



enum Comandos
{
    CMD_FORWARD = 'w',
    CMD_BACKWARD = 's',
    CMD_RIGHT = 'd',
    CMD_LEFT = 'a',
    CMD_STOP = 'q'
}; // Enumeración de comandos recibidos

struct ServoPins
{
    Servo servo;
    int servoPin;
    String servoName;
    int initialPosition;
};
std::vector<ServoPins> servoPins =
    {
        {Servo(), 15, "Base", 90},
        {Servo(), 26, "Shoulder", 90},
        {Servo(), 32, "Elbow", 90},
        {Servo(), 33, "Gripper", 90},
};

struct RecordedStep
{
    int servoIndex;
    int value;
    int delayInStep;
};
std::vector<RecordedStep> recordedSteps;

bool recordSteps = false;
bool playRecordedSteps = false;

unsigned long previousTimeInMilli = millis();

const char *ssid = "Xiaomi";
const char *password = "12345678";

WiFiServer TCPServer(8266); // Servidor del ESP32
WiFiClient TCPClient;       // Cliente TCP (PC)

AsyncWebServer server(80);
AsyncWebSocket wsRobotArmInput("/RobotArmInput");

// Función para manejar la detección del sensor de proximidad
bool detectar_obstaculo()
{
    long duration, distance;
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = (duration / 2) / 29.1;
    return distance < 10; // Ajusta el valor de distancia según tus necesidades
}

void byteReceived(byte byteReceived)
{

    switch (byteReceived)
    {

    case CMD_FORWARD:
        Serial.println("Forward");
        digitalWrite(PIN_MOTOR_R_FWD, HIGH);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, HIGH);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);

        break;

    case CMD_BACKWARD:

        Serial.println("Backward");
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, HIGH);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, HIGH);

        break;

    case CMD_RIGHT:

        Serial.println("Right");
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, HIGH);
        digitalWrite(PIN_MOTOR_L_FWD, HIGH);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);

        break;

    case CMD_LEFT:
        Serial.println("Left");
        digitalWrite(PIN_MOTOR_R_FWD, HIGH);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, HIGH);

        break;
    case CMD_STOP:

        Serial.println("Stop ");

        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);

        break;



    default:
        break;
    }
}



void writeServoValues(int servoIndex, int value)
{
    if (recordSteps)
    {
        RecordedStep recordedStep;
        if (recordedSteps.size() == 0) // We will first record initial position of all servos.
        {
            for (int i = 0; i < servoPins.size(); i++)
            {
                recordedStep.servoIndex = i;
                recordedStep.value = servoPins[i].servo.read();
                recordedStep.delayInStep = 0;
                recordedSteps.push_back(recordedStep);
            }
        }
        unsigned long currentTime = millis();
        recordedStep.servoIndex = servoIndex;
        recordedStep.value = value;
        recordedStep.delayInStep = currentTime - previousTimeInMilli;
        recordedSteps.push_back(recordedStep);
        previousTimeInMilli = currentTime;
    }
    servoPins[servoIndex].servo.write(value);
}

void setUpPinModes()
{
    for (int i = 0; i < servoPins.size(); i++)
    {
        servoPins[i].servo.attach(servoPins[i].servoPin);
        servoPins[i].servo.write(servoPins[i].initialPosition);
    }
}

void setup(void)
{

    pinMode(PIN_MOTOR_R_FWD, OUTPUT);
    pinMode(PIN_MOTOR_R_BWD, OUTPUT);
    pinMode(PIN_MOTOR_L_FWD, OUTPUT);
    pinMode(PIN_MOTOR_L_BWD, OUTPUT);

    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    digitalWrite(PIN_MOTOR_R_FWD, LOW);
    digitalWrite(PIN_MOTOR_R_BWD, LOW);
    digitalWrite(PIN_MOTOR_L_FWD, LOW);
    digitalWrite(PIN_MOTOR_L_BWD, LOW);

    setUpPinModes();
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    TCPServer.begin();


    Serial.println("HTTP server started");
}

void loop()
{
    wsRobotArmInput.cleanupClients();

    /*if (Serial.available() > 0)
        byteReceived(Serial.read());*/

    if (!TCPClient.connected())
    {
        // try to connect to a new client
        TCPClient = TCPServer.available();
    }
    else
    {
        // read data from the connected client
        if (TCPClient.available() > 0)
        {
            byteReceived(TCPClient.read());
        }
    }

    // Detección de proximidad
    if (detectar_obstaculo())
    {
        Serial.println("Obstáculo detectado. Deteniendo el carro.");
        byteReceived(CMD_STOP);
    }
    char buffer[32];
    int len = sizeof(TCPClient.read()-1); 
    if (len > 0) {
    buffer[len] = '\0'; // Null-terminate the string
    int servo_id, value;
    sscanf("%d,%d", &servo_id, &value);
    writeServoValues(servo_id, value);
    printf("Servo %d set to %d\n", servo_id, value);
}
}
