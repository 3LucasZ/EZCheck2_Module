#include <ArduinoJson.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <HTTPClient.h>

#include <Update.h>
#include <Preferences.h>

#include "env.h"
#include "pages.h"

//META
#define VERSION "2.2"
#define DEFAULT_ID "machine-"+millis()

//Comms
WiFiServer wifiServer(80);
WebServer webServer(80);
//const String serverPath = "http://ezserver.local";
const String serverPath = "https://192.168.1.208:3000";
const String signInPath = serverPath+"/api/post/join-machine";
const String signOutPath = serverPath+"/api/post/leave-machine";

//Global
bool sim = true;
bool signedIn = false;
String user = "";
String pass = "";

//Preferences
Preferences preferences; 
int program;
String id;

//Keypad
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pin_rows[ROW_NUM] = {16, 4, 2, 15};
byte pin_column[COLUMN_NUM] = {19, 18, 5, 17};
Keypad pad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

//LCD
const int rs = 32, en = 33, d4 = 25, d5 = 26, d6 = 27, d7 = 14;
LiquidCrystal lcd = LiquidCrystal(rs, en, d4, d5, d6, d7);

//Gasher
const int red = 22; 
const int green = 23;

void setup(){
  //activate
  Serial.begin(115200);
  if (!sim) lcd.begin(16, 2);
  preferences.begin("app", false);
  pinMode(red,OUTPUT); digitalWrite(red,LOW);
  pinMode(green,OUTPUT); digitalWrite(green,LOW);

  //load preferences
  program = preferences.getUInt("program", 0);
  String network = preferences.getString("network", DEFAULT_NETWORK);
  String password = preferences.getString("password", DEFAULT_PASSWORD);
  id = preferences.getString("id", DEFAULT_ID);
  
  //begin wifi 
  if (program==0 || program==1){
    WiFi.begin(network.c_str(), password.c_str());    
  }
  else if (program==2){
    WiFi.softAP(id.c_str(), ADMIN_PASSWORD);
    wifiServer.begin();
  }
  
  //setup routes
  if (program==1){
    updApi();
    webServer.begin();
  }
  
  //connect wifi
  tclear();tprint("Initializing wifi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    tprint(".");
    if (WiFi.status() == WL_CONNECT_FAILED){
      tclear();
      tprint("Failed to connect."); 
      tprint(" Press (A) to restart device in WiFi configuration mode.");
      tprint(" Press (B) to try to reconnect.");
      while (true){
        char key = tread();
        if (key=='A') {
          preferences.putUInt("program",2);
          ESP.restart();
        } else if (key=='B') {
          ESP.restart();
        }
      }
    }
  }
  tclear();
  tprint("Ready!")
  tprint(" IP: ");tprint(WiFi.localIP());
  tprint(" Firmware Version: ");tprint(VERSION);
  tprint(" Program: ";tprint(program);
  delay(1000);
}

void loop() {
  if(program==0) regLoop();
  else if (program==1) updLoop();
  else if (program==2) cfgLoop();
}

void regLoop(){
  char key = tread(); //handle keypress
  if (key){
    if (key=='#') { //# -> sign out
      if (signedIn) signOut();
    } else if (key=='D') { //D -> pop pass
      pass.remove(pass.length()-1);
      tclear();tprint(pass);
    } else if (key=='*') { //* -> submit pass
      if (pass == ADMIN_PASSWORD){
        preferences.putUInt("program",1);
        ESP.restart();
      } else signIn();
    } else { //all other keys -> add char to pass
      pass.concat(key);
      tclear();tprint(pass);
    }
  }
}

void updLoop(){
  webServer.handleClient();
  delay(1);
}

void cfgLoop(){
//  WiFiClient client = server.available();
//  if (client) {  
//    String currentLine = "";     
//  }
}

boolean signIn(){
  //ensure connected 
  if(WiFi.status() == WL_CONNECTED){
    //confirmation
    tclear();tprint("Logging in.");
    //send request
    WiFiClientSecure client;client.setInsecure();HTTPClient http;
    http.begin(client, signInPath);
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(1024);
    doc["machineName"] = id;
    doc["machineSecret"] = API_KEY;
    doc["studentPIN"] = pass;
    doc["IP"] = WiFi.localIP();
    String msg; serializeJson(doc,msg);
    int resCode = http.POST(msg);
    //inspect res
    String res = http.getString();
    DynamicJsonDocument resData(1024);
    deserializeJson(resData, res);    
    if (resCode==200) {
      String tmp = response_data["name"];
      user = tmp;
      pass = "";
      http.end();
      tclear();tprint("Hello ");tprint(user);tprint("!");
      digitalWrite(green,HIGH);
      signedIn = true;
      return true;
    } else {
      user = "";
      pass = "";
      http.end();
      tclear();tprint(res);
      digitalWrite(red,HIGH);
      return false;
    }
  }
  tclear();tprint("WiFi not connected.");
  return false;
}

boolean signOut(){
  //ensure connected 
  if(WiFi.status() == WL_CONNECTED){
    //send request
    WiFiClientSecure client;client.setInsecure();HTTPClient http;
    http.begin(client, signOutPath);
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(1024);
    doc["machineName"] = id;
    doc["machineSecret"] = API_KEY;
    String msg; serializeJson(doc, msg);
    int httpResponseCode = http.POST(msg);
    if (httpResponseCode == 200) {
      return true;
    }
    else {
      tclear();tprint("Error Signing Out: ");tprint(httpResponseCode);
      user = "";
      pass = "";
      http.end();
      return false;
    }
  }
  tclear();tprint("WiFi not connected.");
  return false;
}

void updApi(){
  webServer.on("/", HTTP_GET, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", loginIndex);
  });
  webServer.on("/serverIndex", HTTP_GET, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  webServer.on("/update", HTTP_POST, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        preferences.putUInt("program", 0);
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
} 
char tread(){
  if (!sim) return pad.getKey();
  else if (Serial.available()) return Serial.read();
  else return '\0';
}
void tprint(String x){
  if (!sim) lcd.print(x);
  else Serial.print(x);
}
void tprint(char x){
  if (!sim) lcd.print(x);
  else Serial.print(x);
}
void tprint(int x){
  if (!sim) lcd.print(x);
  else Serial.print(x);
}
void tprint(IPAddress x){
  if (!sim) lcd.print(x);
  else Serial.print(x);
}
void tclear(){
  if (!sim) lcd.clear();
  else Serial.println();
}
