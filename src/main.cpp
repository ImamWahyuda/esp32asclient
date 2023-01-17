#include "SPI.h"
#include <RTCLib.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SPIFFS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "Preferences.h"

char message[50];

int data1, data2, data3, data4;

// Replace with your network credentials
const char *ssidAP = "ESP32AP";
const char *passwordAP = "katakuri";
char ssid[50], password[50];

// Set web server port number to 80
AsyncWebServer server(80);
Preferences preferences;

void APMode()
{
  // Connect to Wi-Fi network with SSID and password
  Serial.println("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssidAP, passwordAP);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accesspoint.html"); });
  server.on("/accesspoint.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accesspoint.css"); });
  server.on("/accesspoint.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accesspoint.js"); });
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              // WiFi.disconnect();
              int ssidIndex = 0;
              int passwordIndex = 1;
              char file_name_char[50];
              String ssidGet =  request->arg(ssidIndex).c_str();;
              String passwordGet = request->arg(passwordIndex).c_str();

              ssidGet.toCharArray(ssid, 50);
              passwordGet.toCharArray(password, 50);

              WiFi.begin(ssid, password);
              unsigned long startTime = millis();
              while (WiFi.status() != WL_CONNECTED && (millis() - startTime) <= 2000)
              {
                delay(500);
                Serial.print(".");
              }

              if(WiFi.status() != 3) {
                return request->send(200, "text/plain", "ssid or password is not valid");
              }
              // Print local IP address and start web server
              Serial.println("");
              Serial.println("WiFi connected.");
              Serial.println("IP address: ");
              String _ip = WiFi.localIP().toString();
              Serial.println(_ip);

              if(_ip != "0.0.0.0") {
                Serial.println("tersimpan");
                preferences.putString("ssid", ssidGet);
                preferences.putString("password", passwordGet);
                WiFi.disconnect();
              }

              request->send(200, "text/plain", _ip); });
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
  digitalWrite(15, HIGH);
}

void setup()
{
  Serial.begin(115200);

  preferences.begin("credential", false);
  String ssidPref = preferences.getString("ssid", "Sony XPERIA XZ2");
  String passwordPref = preferences.getString("password", "katakuri");

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  };

  ssidPref.toCharArray(ssid, 50);
  passwordPref.toCharArray(password, 50);

  Serial.println(ssid);
  Serial.println(password);

  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime) <= 10000)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    APMode();
    while (1)
      ;
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html"); });

  // Route to load style.css file
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.css", "text/css"); });

  server.on("/chart.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/chart.min.js"); });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.js"); });

  server.on("/get-data", HTTP_GET, [](AsyncWebServerRequest *request)
            { return request->send(200, "text/plain", message); });

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  // Start server
  server.begin();
}

void loop()
{
  float data1 = random(600, 1200);
  float data2 = random(214, 723);
  float data3 = random(352, 9217);
  float data4 = random(1, 10);
  sprintf(message, "%f,%f,%f,%f\r\n", data1, data2, data3, data4);
}