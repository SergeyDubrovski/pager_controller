/*
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoWebsockets.h>

const char *ssid = "UltraLab_Work";
const char *password = "Minsk@Ultra";
const char *websockets_server_host = "91.215.176.34"; // Enter server adress
const uint16_t websockets_server_port = 7012;       // Enter server port

using namespace websockets;

AsyncWebServer server(80);
WebsocketsClient client;
String message2 = "Hello1";
String newMess = "";

LiquidCrystal_I2C lcd(0x27, 16, 2);
void onMessageCallback(WebsocketsMessage message)
{
  lcd.clear();
  if (message.length() > 0)
  {
    lcd.backlight();
    newMess = message.data();

    lcd.setCursor(0, 0);
    lcd.print(newMess.substring(0, 17));
    lcd.setCursor(0, 1);
    lcd.print(newMess.substring(17));

  } else {
    lcd.clear();
    lcd.noBacklight();
  }
}

void setup(void)
{
  IPAddress ip(10, 10, 10, 248);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gateway(10, 10, 10, 1);

  Serial.begin(9600);
  //WiFi.config(ip, gateway, subnet);
   WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // LiquidCrystal_I2C lcd(0x27, 16, 2);

  lcd.init();

  lcd.setCursor(0, 1);
  lcd.print("Hello, world");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Hi! I am ESP8266."); });

  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  client.connect(websockets_server_host, websockets_server_port, "/api/esp");

  client.onMessage(onMessageCallback);
}

void loop(void)
{
  client.poll();

 client.connect(websockets_server_host, websockets_server_port, "/api/esp");

  client.send("Hello Server" + String(2));

  delay(1000);
}*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <LiquidCrystal_I2C.h>

WebSocketsClient webSocket;

const char *ssid = "TP-Link_5306"; // UltraLab
const char *password = "Serg1990"; // Minsk@Ultra

unsigned long messageInterval = 5000;
bool connected = false;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DEBUG_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    DEBUG_SERIAL.printf("[WSc] Disconnected!\n");
    connected = false;
    break;
  case WStype_CONNECTED:
  {
    DEBUG_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
    connected = true;
    // send message to server when Connected
    DEBUG_SERIAL.println("[WSc] SENT: Connected");
    webSocket.sendTXT("Connected");
  }
  break;
  case WStype_TEXT:
    DEBUG_SERIAL.printf("[WSc] RESPONSE: %s\n", payload);
    webSocket.sendTXT(payload);
    String str = (char *)payload;
    lcd.clear();
    if (str.length() > 0)
    {
      lcd.backlight();

      lcd.setCursor(0, 0);
      lcd.print(str.substring(0, 16));
      lcd.setCursor(0, 1);
      lcd.print(str.substring(16));
    }
    else
    {
      lcd.clear();
      lcd.noBacklight();
    }

    break;
    /*
  case WStype_BIN:
    DEBUG_SERIAL.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);
    break;
  case WStype_PING:
    // pong will be send automatically
    DEBUG_SERIAL.printf("[WSc] get ping\n");
    break;
  case WStype_PONG:
    // answer to a ping we send
    DEBUG_SERIAL.printf("[WSc] get pong\n");
    break;*/
  }
}

void setup()
{
  DEBUG_SERIAL.begin(9600);

  //  DEBUG_SERIAL.setDebugOutput(true);

  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--)
  {
    DEBUG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    DEBUG_SERIAL.flush(); 
    delay(1000);
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  DEBUG_SERIAL.print("Local IP: ");
  DEBUG_SERIAL.println(WiFi.localIP());
  // server address, port and URL
  webSocket.begin("oxsedu.site", 81, "/api/esp");

  // event handler
  webSocket.onEvent(webSocketEvent);
  lcd.init();

  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
}

unsigned long lastUpdate = millis();

void loop()
{
  webSocket.loop();
  if (connected && lastUpdate + messageInterval < millis())
  {
    DEBUG_SERIAL.println("[WSc] SENT: Simple js client message!!");
    webSocket.sendTXT("Simple js client message!!");

    lastUpdate = millis();
  }
}