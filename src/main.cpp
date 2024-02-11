/*

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <LiquidCrystal_I2C.h>
#include <LittleFS.h>

WebSocketsClient webSocket;

const char *ssid = "TP-Link_5306"; // UltraLab
const char *password = "Serg1990"; // Minsk@Ultra

unsigned long messageInterval = 5000;
bool connected = false;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    connected = false;
    break;
  case WStype_CONNECTED:
  {
    Serial.printf("[WSc] Connected to url: %s\n", payload);
    connected = true;
    // send message to server when Connected
    Serial.println("[WSc] SENT: Connected");
    webSocket.sendTXT("Connected");
  }
  break;
  case WStype_TEXT:
    Serial.printf("[WSc] RESPONSE: %s\n", payload);
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

  }
}

void setup()
{
  Serial.begin(9600);

  //------
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  File file = LittleFS.open("/data", "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    Serial.println(LittleFS.exists("/data/text.txt"));
    Serial.println(LittleFS.exists("/text.txt"));
    // Serial.println(LittleFS.format());
  }

  Serial.println("File Content:");
  while (file.available())
  {
    Serial.write(file.read());
  }

  Dir dir = LittleFS.openDir("/");
  Serial.print(dir.fileName());
  while (dir.next())
  {
    Serial.print(dir.fileName());
    if (dir.fileSize())
    {
      File f = dir.openFile("r");
      Serial.println(f.size());
    }
  }

  file.close();

  //-------

  //  DEBUG_SERIAL.setDebugOutput(true);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  // server address, port and URL
  webSocket.begin("oxsedu.site", 81, "/api/esp");

  // event handler
  webSocket.onEvent(webSocketEvent);
  lcd.init();

  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
}

// unsigned long lastUpdate = millis();

void loop()
{
  webSocket.loop();
  if (connected) //&& lastUpdate + messageInterval < millis()
  {
    // Serial.println("[WSc] SENT: Simple js client message!!");
    // webSocket.sendTXT("Simple js client message!!");

    // lastUpdate = millis();
  }
}*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "ESP8266WebServer.h"

const char *ssid = "TP-Link_5306"; // UltraLab
const char *password = "Serg1990"; // Minsk@Ultra

unsigned long messageInterval = 5000;
bool connected = false;
LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WebServer server(81);
String message = "";

void standartMessage()
{
  lcd.clear();
  lcd.noBacklight();
  lcd.setCursor(0, 0);
  lcd.print("Im ready for mess.");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
}
void handleSpecificArg()
{

  message += server.arg("message");
  server.send(200, "text/plain", message);
  if (message.length() > 0)
  {
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print(message.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(message.substring(16));
    message = "";
    delay(30000);
    standartMessage();
  }
}

void setup()
{
  Serial.begin(9600);

  IPAddress local_IP(192, 168, 1, 106);

  IPAddress gateway(192, 168, 1, 1);

  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(192, 168, 1, 1); // optional
  IPAddress secondaryDNS(8, 8, 8, 8);   // optional

  WiFi.begin(ssid, password);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  server.on("/", handleSpecificArg);
  server.begin();
  lcd.init();
  standartMessage();
}

// unsigned long lastUpdate = millis();

void loop()
{
  server.handleClient();
}