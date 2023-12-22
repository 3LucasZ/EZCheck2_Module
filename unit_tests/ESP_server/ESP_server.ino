#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <HTTPClient.h>

#include <Update.h>

WebServer webServer(80);

void setup(){
  Serial.begin(115200);
  WiFi.begin("DubHub21", "#0Ssup#r06291969");    
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  createWebServerApi();
  webServer.begin(); 
  Serial.println(WiFi.localIP());
}

void loop() {
  webServer.handleClient();
  delay(1);
}

void createWebServerApi(){
  webServer.on("/", HTTP_GET, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", "Hello World");
    Serial.println("Received request");
  });
} 
