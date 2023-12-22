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
  delay(1000);
  sendReq();
  delay(1000);
}

void sendReq(){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClientSecure client;client.setInsecure();HTTPClient http;
    if (http.begin(client, "https://192.168.1.208:3000/api/module/ping")){
      if (http.GET() > 0){
        String data = http.getString();
        Serial.print("client: ");Serial.println(data);
      } else {
        Serial.println("Nothin.");
      }
    } else {
      Serial.println("Error occured in https.begin");
    }
    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
}
void createWebServerApi(){
  webServer.on("/", HTTP_GET, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", "Hello World");
    Serial.print("server: ");Serial.println("/");
  });
} 
