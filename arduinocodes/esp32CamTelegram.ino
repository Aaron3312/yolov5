#include "WifiCam.hpp"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

static const char* WIFI_SSID = "Xiaomi";
static const char* WIFI_PASS = "12345678";

esp32cam::Resolution initialResolution;
// Initialize Telegram BOT
#define BOTtoken "6757159977:AAGLNvl4qRNq7fBWpAPFC8cu7tKqMrnpD2E"  // your Bot Token (Get from Botfather)

#define CHAT_ID "1269181183"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

WebServer server(80);

// Pin del LED del ESP32-CAM
const int ledPin = 4; // Cambia el número de pin según la configuración de tu ESP32-CAM

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;


  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/Ips to get the IP of the ESP32-CAM \n";
      welcome += "/photo to get a photo from the ESP32-CAM \n";
      bot.sendMessage(chat_id, welcome, "");
    }




void setupLed() {
  pinMode(ledPin, OUTPUT);
}

void setLedIntensity(int intensity) {
  analogWrite(ledPin, map(intensity, 0, 10, 0, 255));
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  setupLed(); // Inicializar el LED

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi failure");
    delay(5000);
    ESP.restart();
  }
  Serial.println("WiFi connected");

  {
    using namespace esp32cam;

    initialResolution = Resolution::find(1024, 768);

    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(initialResolution);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    if (!ok) {
      Serial.println("camera initialize failure");
      delay(5000);
      ESP.restart();
    }
    Serial.println("camera initialize success");
  }

  Serial.println("camera starting");
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  server.handleClient();
}


