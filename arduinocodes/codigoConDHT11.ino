#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

//dht11
#include "DHTesp.h"


#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <ESP32Servo.h>
#include <iostream>
#include <sstream>

int pinDHT = 25;
DHTesp dht;

const int PWMFreq = 1000; /* 1 KHz */
const int PWMSpeedChannel = 2;
const int PWMResolution = 8; /* 8 bits for PWM resolution 0 - 255 */

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define STOP 0

bool ignora;
// Definiciones de pines usados para los motores
#define PIN_MOTOR_R_FWD 5
#define PIN_MOTOR_R_BWD 18
#define PIN_MOTOR_L_FWD 19
#define PIN_MOTOR_L_BWD 21      

// Definiciones de comandos para el carro
enum Comandos { CMD_FORWARD = 's', CMD_BACKWARD ='w', CMD_RIGHT ='d', CMD_LEFT = 'a', CMD_STOP = 'q', CMD_DESPACIO = 'e' , CMD_DESPACIOA = 'x', CMD_DESPACIODrch = 'f', CMD_DESPACIOizq = 'g', CMD_Ignora = 'l', CMD_NoIgnora = 'k'};

// Definiciones de comandos para el brazo robótico
struct ServoPins
{
  Servo servo;
  int servoPin;
  String servoName;
  int initialPosition;  
};

// Definiciones de comandos para el brazo robótico
std::vector<ServoPins> servoPins = 
{
  { Servo(), 15 , "Base", 90},
  { Servo(), 26 , "Shoulder", 120},
  { Servo(), 32 , "Elbow", 90},
  { Servo(), 33 , "Gripper", 90},
};


// Definiciones de comandos para guardar los pasos para el brazo robótico
struct RecordedStep
{
  int servoIndex;
  int value;
  int delayInStep;
};
// Definiciones de comandos para guardar los pasos para el brazo robótico
std::vector<RecordedStep> recordedSteps;

//los valores de guardar los pasos y reproducirlos
bool recordSteps = false;
bool playRecordedSteps = false;

//para guardar el tiempo en milisegundos
unsigned long previousTimeInMilli = millis();

//Configuración de red
const char* ssid     = "Xiaomi";
const char* password = "12345678";

//Configuración de red TCP para el carro
WiFiServer TCPServer(8266); //Servidor del ESP32
WiFiClient TCPClient; //Cliente TCP (PC)


// Initialize Telegram BOT
#define BOTtoken "6923908630:AAFF9qPfB0rSwWakBMuY4tSZ00hHcQpULxE"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1269181183"

const int ledPin1 = 27;
bool ledState1 = LOW;

//bot de telegram
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 3000;
unsigned long lastTimeBotRan;


//Configuración de red TCP para el brazo robótico
AsyncWebServer server(80);
AsyncWebSocket wsRobotArmInput("/RobotArmInput");
AsyncWebSocket wsCarInput("/CarInput");
//pagina web html para el brazo robotico y el carro con el joystick?
const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>

    input[type=button]
    {
      background-color:red;color:white;border-radius:30px;width:100%;height:40px;font-size:20px;text-align:center;
    }
    .arrows {
      font-size:30px;
      color:greenyellow;
    }

    td.button {
      background-color:blue;
      border-radius:25%;
      box-shadow: 5px 5px #888888;
    }

    td.button:active {
      transform: translate(5px,5px);
      box-shadow: none; 
    }


        
    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
         -khtml-user-select: none; /* Konqueror HTML */
           -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
                user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
    }

    .slidecontainer {
      width: 100%;
    }

    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 20px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }

    .slider:hover {
      opacity: 1;
    }
  
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    .slider::-moz-range-thumb {
      width: 40px;
      height: 40px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    </style>
  
  </head>
  <body class="noselect" align="center" style="background-color:white">
     
    <h1 style="color: rebeccapurple;text-align:center;">Proyecto Carro-Brazo by Aaron Hernandez</h1>
    <h2 style="color: rebeccapurple;text-align:center;">IOT Car and arm Controls</h2>
    
    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Garra:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Gripper" oninput='sendButtonInput("Gripper",value)'>
          </div>
        </td>
      </tr> 
      <tr/><tr/>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Codo:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Elbow" oninput='sendButtonInput("Elbow",value)'>
          </div>
        </td>
      </tr> 
      <tr/><tr/>      
      <tr>
        <td style="text-align:left;font-size:25px"><b>Hombro:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="0" max="220" value="90" class="slider" id="Shoulder" oninput='sendButtonInput("Shoulder",value)'>
          </div>
        </td>
      </tr>  
      <tr/><tr/>      
      <tr>
        <td style="text-align:left;font-size:25px"><b>Base:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="0" max="180" value="90" class="slider" id="Base" oninput='sendButtonInput("Base",value)'>
          </div>
        </td>
      </tr> 
      <tr/><tr/> 
      <tr>
        <td style="text-align:left;font-size:25px"><b>Record:</b></td>
        <td><input type="button" id="Record" value="OFF" ontouchend='onclickButton(this)'></td>
        <td></td>
      </tr>
      <tr/><tr/> 
      <tr>
        <td style="text-align:left;font-size:25px"><b>Play:</b></td>
        <td><input type="button" id="Play" value="OFF" ontouchend='onclickButton(this)'></td>
        <td></td>
      </tr>
      <tr/><tr/>
      <tr>
        <td style="text-align:left;font-size:25px"><b>Speed:</b></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="0" max="255" value="150" class="slider" id="Speed" oninput='sendButtonInput("Speed",value)'>
          </div>
        </td>
        </tr>  
        <tr/><tr/>
    </tr> 
    <tr>
      <td></td>
      <td class="button" ontouchstart='sendButtonInput("MoveCar","1")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8679;</span></td>
      <td></td>
    </tr>
    <tr>
      <td class="button" ontouchstart='sendButtonInput("MoveCar","3")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8678;</span></td>
      <td class="button"></td>    
      <td class="button" ontouchstart='sendButtonInput("MoveCar","4")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8680;</span></td>
    </tr>
    <tr>
      <td></td>
      <td class="button" ontouchstart='sendButtonInput("MoveCar","2")' ontouchend='sendButtonInput("MoveCar","0")'><span class="arrows" >&#8681;</span></td>
      <td></td>
    </tr>
    <tr/><tr/>
    <tr>
    </table>
    
  
    <script>
      var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/CarInput";
      var webSocketRobotArmInputUrl = "ws:\/\/" + window.location.hostname + "/RobotArmInput";      
      var websocketCarInput;
      var websocketRobotArmInput;

        //cambios para el carro 
        function initCarInputWebSocket()
        {
        websocketCarInput = new WebSocket(webSocketCarInputUrl);
        websocketCarInput.onopen    = function(event)
        {
          sendButtonInput("Speed", document.getElementById("Speed").value);                   
        };
        websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};
        websocketCarInput.onmessage = function(event){};        
      }
      function initWebSocket() 
      {
        initCarInputWebSocket();
        initRobotArmInputWebSocket();
      }
      
      function initRobotArmInputWebSocket() 
      {
        websocketRobotArmInput = new WebSocket(webSocketRobotArmInputUrl);
        websocketRobotArmInput.onopen    = function(event){};
        websocketRobotArmInput.onclose   = function(event){setTimeout(initRobotArmInputWebSocket, 2000);};
        websocketRobotArmInput.onmessage    = function(event)
        {
          var keyValue = event.data.split(",");
          var button = document.getElementById(keyValue[0]);
          button.value = keyValue[1];
          if (button.id == "Record" || button.id == "Play")
          {
            button.style.backgroundColor = (button.value == "ON" ? "green" : "red");  
            enableDisableButtonsSliders(button);
          }
        };
      }
      
      function sendButtonInput(key, value) 
      {
        var data = key + "," + value;
        websocketRobotArmInput.send(data);
        websocketCarInput.send(data);
      }
      
      function onclickButton(button) 
      {
        button.value = (button.value == "ON") ? "OFF" : "ON" ;        
        button.style.backgroundColor = (button.value == "ON" ? "green" : "red");          
        var value = (button.value == "ON") ? 1 : 0 ;
        sendButtonInput(button.id, value);
        enableDisableButtonsSliders(button);
      }
      
      function enableDisableButtonsSliders(button)
      {
        if(button.id == "Play")
        {
          var disabled = "auto";
          if (button.value == "ON")
          {
            disabled = "none";            
          }
          document.getElementById("Gripper").style.pointerEvents = disabled;
          document.getElementById("Elbow").style.pointerEvents = disabled;          
          document.getElementById("Shoulder").style.pointerEvents = disabled;          
          document.getElementById("Base").style.pointerEvents = disabled; 
          document.getElementById("Record").style.pointerEvents = disabled;
        }
        if(button.id == "Record")
        {
          var disabled = "auto";
          if (button.value == "ON")
          {
            disabled = "none";            
          }
          document.getElementById("Play").style.pointerEvents = disabled;
        }        
      }
           
      window.onload = initWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });      
    </script>
  </body>    
</html>
)HTMLHOMEPAGE";

//esto es para cosas del html y hace 
void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}
//
void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}


// Handle what happens when you receive new messages
bool handleNewMessages(int numNewMessages, String x, bool ignora) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  ignora = ignora;

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    /*
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }*/
    
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/led_on to turn GPIO ON \n";
      welcome += "/led_off to turn GPIO OFF \n";
      welcome += "/state to request current GPIO state \n";
      welcome += "/x to send te ip address \n";
      welcome += "/Temperatura to send the temperature \n";
      welcome += "/Humedad to send the humidity \n";
      welcome += "/Ignora_Proxi to ignore the proximity sensor \n";
      welcome += "/NoIgnora_Proxi to not ignore the proximity sensor \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if(text == "/x"){
      bot.sendMessage(chat_id, x,"");
      return ignora;
    }
    if(text == "/Temperatura"){
    //Obtenemos el arreglo de datos (humedad y temperatura)
    TempAndHumidity data = dht.getTempAndHumidity();
    //Mostramos los datos de la temperatura y humedad
      bot.sendMessage(chat_id, "Temperatura: " + String(data.temperature, 2) + "°C","");
        return ignora;
    }

    if(text == "/Humedad"){
    //Obtenemos el arreglo de datos (humedad y temperatura)
    TempAndHumidity data = dht.getTempAndHumidity();
    //Mostramos los datos de la temperatura y humedad
    bot.sendMessage(chat_id, "Humedad: " + String(data.humidity, 1) + "%","");
    return ignora;
    }
    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      ledState1 = HIGH;
      digitalWrite(ledPin1, ledState1);
        return ignora;
    }
    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      ledState1 = LOW;
      digitalWrite(ledPin1, ledState1);
        return ignora;
    }
    if (text == "/state") {
      String state = "Current LED state = " + String(ledState1);
      bot.sendMessage(chat_id, state, "");
        return ignora;
    }
    if (text == "/Ignora_Proxi") {
      String state = "Proximity sensor ignorated set to ON";
      bot.sendMessage(chat_id, state, "");
      ignora = true;
        return true;
    }
    if (text == "/NoIgnora_Proxi") {
      String state = "Proximity sensor ignorated set to OFF";
      bot.sendMessage(chat_id, state, "");
      ignora = false;
        return false;
    }

    
  }
}



//esto es para el carro en la pagina web


void moveCar(int inputValue)
{
    printf("Input Value: %d\n", inputValue);
    switch (inputValue)
    {
    case UP:
    digitalWrite(PIN_MOTOR_R_FWD, LOW);
    digitalWrite(PIN_MOTOR_R_BWD, HIGH);
    digitalWrite(PIN_MOTOR_L_FWD, LOW);
    digitalWrite(PIN_MOTOR_L_BWD, HIGH);

    break;
    case DOWN:
    digitalWrite(PIN_MOTOR_R_FWD, HIGH);
    digitalWrite(PIN_MOTOR_R_BWD, LOW);
    digitalWrite(PIN_MOTOR_L_FWD, HIGH);
    digitalWrite(PIN_MOTOR_L_BWD, LOW);

    break;
    case LEFT:
    digitalWrite(PIN_MOTOR_R_FWD, HIGH);
    digitalWrite(PIN_MOTOR_R_BWD, LOW);
    digitalWrite(PIN_MOTOR_L_FWD, LOW);
    digitalWrite(PIN_MOTOR_L_BWD, HIGH);
    break;
    case RIGHT:
    digitalWrite(PIN_MOTOR_R_FWD, LOW);
    digitalWrite(PIN_MOTOR_R_BWD, HIGH);
    digitalWrite(PIN_MOTOR_L_FWD, HIGH);
    digitalWrite(PIN_MOTOR_L_BWD, LOW);    
    break;
    case STOP:
    digitalWrite(PIN_MOTOR_R_FWD, LOW);
    digitalWrite(PIN_MOTOR_R_BWD, LOW);
    digitalWrite(PIN_MOTOR_L_FWD, LOW);
    digitalWrite(PIN_MOTOR_L_BWD, LOW);
    break;
    default:
    digitalWrite(PIN_MOTOR_R_FWD, LOW);
    digitalWrite(PIN_MOTOR_R_BWD, LOW);
    digitalWrite(PIN_MOTOR_L_FWD, LOW);
    digitalWrite(PIN_MOTOR_L_BWD, LOW);
    break;
    }
}

void onCarInputWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        int valueInt = atoi(value.c_str());     
        if (key == "MoveCar")
        {
          moveCar(valueInt);        
        }
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}



//esto es para el brazo robotico en la pagina web
void onRobotArmInputWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      sendCurrentRobotArmState();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');

        int valueInt = atoi(value.c_str()); 
        
        if (key == "Record")
        {
          recordSteps = valueInt;
          if (recordSteps)
          {
            recordedSteps.clear();
            previousTimeInMilli = millis();
          }
        }  
        else if (key == "Play")
        {
          playRecordedSteps = valueInt;
        }
        else if (key == "Base")
        {
          writeServoValues(0, valueInt);            
        } 
        else if (key == "Shoulder")
        {
          writeServoValues(1, valueInt);           
        } 
        else if (key == "Elbow")
        {
          writeServoValues(2, valueInt);           
        }         
        else if (key == "Gripper")
        {
          writeServoValues(3, valueInt);     
        }   
             
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}

//esto actualiza el estado del brazo robotico en la pagina web 
void sendCurrentRobotArmState()
{
  for (int i = 0; i < servoPins.size(); i++)
  {
    wsRobotArmInput.textAll(servoPins[i].servoName + "," + servoPins[i].servo.read());
  }
  wsRobotArmInput.textAll(String("Record,") + (recordSteps ? "ON" : "OFF"));
  wsRobotArmInput.textAll(String("Play,") + (playRecordedSteps ? "ON" : "OFF"));  
}

//esto es para hacer que el brazo robotico se mueva, escribe los valores de los servos.
void writeServoValues(int servoIndex, int value)
{
  if (recordSteps) //en caso de que se este grabando los pasos del brazo robotico se guardan los valores de los servos
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

//esto es para reproducir los pasos del brazo robotico. 
void playRecordedRobotArmSteps()
{
  if (recordedSteps.size() == 0)
  {
    return;
  }
  //This is to move servo to initial position slowly. First 4 steps are initial position    
  for (int i = 0; i < 4 && playRecordedSteps; i++)
  {
    RecordedStep &recordedStep = recordedSteps[i];
    int currentServoPosition = servoPins[recordedStep.servoIndex].servo.read();
    while (currentServoPosition != recordedStep.value && playRecordedSteps)  
    {
      currentServoPosition = (currentServoPosition > recordedStep.value ? currentServoPosition - 1 : currentServoPosition + 1); 
      servoPins[recordedStep.servoIndex].servo.write(currentServoPosition);
      wsRobotArmInput.textAll(servoPins[recordedStep.servoIndex].servoName + "," + currentServoPosition);
      delay(50);
    }
  }
  delay(2000); // Delay before starting the actual steps.
  for (int i = 4; i < recordedSteps.size() && playRecordedSteps ; i++)
  {
    RecordedStep &recordedStep = recordedSteps[i];
    delay(recordedStep.delayInStep);
    servoPins[recordedStep.servoIndex].servo.write(recordedStep.value);
    wsRobotArmInput.textAll(servoPins[recordedStep.servoIndex].servoName + "," + recordedStep.value);
  }
}

//esto es para algo de los servos, no estoy seguro de que hace pero es necesario para que funcione el brazo robotico
void setUpPinModes()
{

  for (int i = 0; i < servoPins.size(); i++)
  {
    servoPins[i].servo.attach(servoPins[i].servoPin);
    servoPins[i].servo.write(servoPins[i].initialPosition);    
  }


  //moveCar(0);
}

//esto es para el carro, recibe un byte y dependiendo del byte hace algo diferente, como moverse o detenerse, etc.
void byteReceived(byte byteReceived) {    
    

    switch(byteReceived){

      //para que el carro se mueva despacio a la izquierda
      case CMD_DESPACIOizq:
        digitalWrite(PIN_MOTOR_R_FWD, HIGH);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, HIGH);
        delay(100);
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);
        delay(100);
      break;
      //para que el carro se mueva despacio a la derecha
      case CMD_DESPACIODrch:
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, HIGH);
        digitalWrite(PIN_MOTOR_L_FWD, HIGH);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);
        delay(100);
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);
        delay(100);
      break;
      //para que el carro se mueva despacio hacia atras
      case CMD_DESPACIOA:
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, HIGH);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, HIGH);
        delay(50);
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);
      break;
      //para que el carro se mueva despacio hacia adelante
      case CMD_DESPACIO:
        digitalWrite(PIN_MOTOR_R_FWD, HIGH);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, HIGH);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);
        delay(50);
        digitalWrite(PIN_MOTOR_R_FWD, LOW);
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);
      break;

      //para que el carro se mueva hacia adelante
      case CMD_FORWARD:
       Serial.println("Forward");
       digitalWrite(PIN_MOTOR_R_FWD, HIGH);
       digitalWrite(PIN_MOTOR_R_BWD, LOW);
       digitalWrite(PIN_MOTOR_L_FWD, HIGH);
       digitalWrite(PIN_MOTOR_L_BWD, LOW);
      break;

      //para que el carro se mueva hacia atras
      case CMD_BACKWARD:
         Serial.println("Backward");    
       digitalWrite(PIN_MOTOR_R_FWD, LOW);
       digitalWrite(PIN_MOTOR_R_BWD, HIGH);
       digitalWrite(PIN_MOTOR_L_FWD, LOW);
       digitalWrite(PIN_MOTOR_L_BWD, HIGH);
      break;

      //para que el carro se mueva a la derecha
      case CMD_RIGHT:
         Serial.println("Right");
       digitalWrite(PIN_MOTOR_R_FWD, LOW);
       digitalWrite(PIN_MOTOR_R_BWD, HIGH);
       digitalWrite(PIN_MOTOR_L_FWD, HIGH);
       digitalWrite(PIN_MOTOR_L_BWD, LOW);
      break;

      //para que el carro se mueva a la izquierda
      case CMD_LEFT:
            Serial.println("Left");
       digitalWrite(PIN_MOTOR_R_FWD, HIGH);
       digitalWrite(PIN_MOTOR_R_BWD, LOW);
       digitalWrite(PIN_MOTOR_L_FWD, LOW);
       digitalWrite(PIN_MOTOR_L_BWD, HIGH);
      break;

      //para que el carro se detenga
      case CMD_STOP:
        Serial.println("Stop");
        digitalWrite(PIN_MOTOR_R_FWD, LOW); 
        digitalWrite(PIN_MOTOR_R_BWD, LOW);
        digitalWrite(PIN_MOTOR_L_FWD, LOW);
        digitalWrite(PIN_MOTOR_L_BWD, LOW);
      break;

      default: break;
    }
}



//esto es el setup para el proyecto, se inicializan los pines y se conecta a la red wifi, al final se inicia el servidor
void setup() {
    setUpPinModes();
  // Configuración de los pines de los motores
  pinMode(PIN_MOTOR_R_FWD, OUTPUT);
  pinMode(PIN_MOTOR_R_BWD, OUTPUT);
  pinMode(PIN_MOTOR_L_FWD, OUTPUT);
  pinMode(PIN_MOTOR_L_BWD, OUTPUT);

  // Inicialización de los pines de los motores
  digitalWrite(PIN_MOTOR_R_FWD, LOW);
  digitalWrite(PIN_MOTOR_R_BWD, LOW);
  digitalWrite(PIN_MOTOR_L_FWD, LOW);
  digitalWrite(PIN_MOTOR_L_BWD, LOW);
  
  //inicializamos un el puerto digital 2 y el 4 para detener el carro si es high
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  pinMode(13, INPUT);
    bool ignora = true;
    //dht
    dht.setup(pinDHT, DHTesp::DHT11); 

    
  pinMode(ledPin1, OUTPUT);
  digitalWrite(ledPin1, ledState1);
  
  Serial.begin(115200);

  Serial.printf("Conectando a: %s\n", ssid);
 
  WiFi.begin(ssid, password);
  
  // Intentamos que se conecte a la red wifi
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando...");
    delay(2000);
    }
  
  Serial.print("Conectado.  ");
  Serial.print(" Dirección IP del módulo: ");
  Serial.println(WiFi.localIP()); //esto deberiamos sacarlo al html o a un display

    //configuracion del bot de telegram
    Serial.println("Iniciando bot de telegram");
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org



  // Configuración del servidor web
  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
      

    // Configuración del servidor web socket para el carro
    wsCarInput.onEvent(onCarInputWebSocketEvent);
    server.addHandler(&wsCarInput);

  wsRobotArmInput.onEvent(onRobotArmInputWebSocketEvent);
  server.addHandler(&wsRobotArmInput);

  server.begin();
  Serial.println("HTTP server started");

 TCPServer.begin();

}

//este es el loop del proyecto, se ejecuta constantemente, se encarga de recibir los bytes y ejecutar las funciones correspondientes
void loop() {
    
    //necesario para optener la ip del cliente
    if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    String x = WiFi.localIP().toString();

    while(numNewMessages) {
      Serial.println("got response");
      ignora = handleNewMessages(numNewMessages,x,ignora);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
    }


    if (ignora == false){ //interruptor para usar esas madres
    // Detección de proximidad con el sensor ultrasónico unicamente recibe valor de 2 sensores, en high o low
    if (digitalRead(13) == LOW) //si los 2 sensores no estan activos el carro se mueve normal
    {
    if(digitalRead(2) == HIGH){ //si el sensor de la izquierda esta activo el carro se mueve a la derecha
      byteReceived(CMD_STOP);
      byteReceived(CMD_DESPACIODrch);
    }
    if(digitalRead(4) == HIGH){ //si el sensor de la derecha esta activo el carro se mueve a la izquierda
      byteReceived(CMD_STOP);
      byteReceived(CMD_DESPACIOizq);
    }
    }
    else if (digitalRead(4) == HIGH && digitalRead(2) == HIGH) //si los 2 sensores estan activos el carro se detiene
    {
      byteReceived(CMD_STOP);
      byteReceived(CMD_FORWARD);
    }}

  
   /*if (Serial.available() > 0)
       byteReceived(Serial.read());*/
    
    if (!TCPClient.connected()) {
        // try to connect to a new client
        TCPClient = TCPServer.available();
    } else {
        // read data from the connected client
        if (TCPClient.available() > 0) {
            Serial.println("Recibiendo datos del cliente TCP");
            byteReceived(TCPClient.read());
                                                                                                                                                                                                                                                                                                                                                                                 
            
        }
    }
/*
        // Detección de proximidad
    if (detectar_obstaculo()) {
        Serial.println("Obstáculo detectado. Deteniendo el carro.");
        byteReceived(CMD_STOP);
    }*/
    wsRobotArmInput.cleanupClients();
  if (playRecordedSteps)
  { 
    playRecordedRobotArmSteps();
  }

}
