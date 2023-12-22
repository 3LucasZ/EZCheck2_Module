#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <HTTPClient.h>

#include <Update.h>

void setup(){
  Serial.begin(115200);
  WiFi.begin("DubHub21", "#0Ssup#r06291969");    
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  sendReq();
  delay(2000);
}

void sendReq(){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClientSecure client;client.setInsecure();HTTPClient http;
    if (http.begin(client, "https://192.168.1.208:3000/api/module/ping")){
      if (http.GET() > 0){
        String data = http.getString();
        Serial.println(data);
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
