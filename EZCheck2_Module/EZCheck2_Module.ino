#include <ArduinoJson.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "env.h"
#include <Preferences.h>

//HTTP
const String serverPath = "http://ezserver.local";
const String signInPath = serverPath+"/join-machine";
const String signOutPath = serverPath+"/leave-machine";

//Global
bool signedIn = false;
String user = "";
String pass = "";

//Keypad
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte pin_rows[ROW_NUM] = {16, 4, 2, 15};
byte pin_column[COLUMN_NUM] = {19, 18, 5, 17};
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

//LCD
const int rs = 32, en = 33, d4 = 25, d5 = 26, d6 = 27, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Gasher
const int red = 22; 
const int green = 23;

void setup(){
  //activate
  Serial.begin(115200);
  WiFi.begin(NETWORK, PASSWORD);
  lcd.begin(16, 2);
  pinMode(red,OUTPUT); digitalWrite(red,LOW);
  pinMode(green,OUTPUT); digitalWrite(green,LOW);
  
  //setup wifi
  lcd.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }
  lcd.clear();lcd.print("Connected");
  delay(1000);
  lcd.clear();lcd.print("Welcome!");
}

void loop(){
  //handle keypress
  char key = keypad.getKey();
  if (key){
    //# -> sign out
    if (signedIn){
      if (key=='#') {
        if (signOut()) {
          digitalWrite(green, LOW);
          lcd.clear();
          lcd.print("Signed Out");
          signedIn = !signedIn;
        }
      }
    }
    else {
      //D -> pop pass
      if (key=='D') {
        pass.remove(pass.length()-1);
        //display
        lcd.clear();
        lcd.print(pass);
      }
      //* -> submit pass
      else if (key=='*') {
        lcd.clear();
        //make request
        boolean res = signIn();
        // correct pass
        if (res) {
          String message = "Hello ";message.concat(user);
          lcd.clear();lcd.print(message);
          digitalWrite(green,HIGH);
          signedIn = true;
        }
        //wrong pass
        else lcd.print("Denied Access");
        pass="";
      }
      //else -> add char to pass
      else {
        pass.concat(key);
        //display
        lcd.clear();
        lcd.print(pass);
        Serial.println(pass);
      }
    }
  }
}

boolean signIn(){
  //connected 
  if(WiFi.status() == WL_CONNECTED){
    //confirmation
    lcd.clear();lcd.print("Logging in");
    //send post request
    WiFiClient client;HTTPClient http;
    http.begin(client, signInPath);
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(1024);
    doc["machineName"] = ID;
    doc["machineSecret"] = SECRET;
    String msg; serializeJson(doc,msg);
    int httpResponseCode = http.POST(msg);
    //valid request
    String response = http.getString();
    DynamicJsonDocument response_data(1024);
    deserializeJson(response_data, response);
    bool auth = response_data["authorized"];
    
    if (auth) {
      String tmp = response_data["name"];
      user = tmp;
      pass = "";
      http.end();
      return true;
    }
    else {
      lcd.clear();lcd.print("Error: ");lcd.print(httpResponseCode);
      user = "";
      pass = "";
      http.end();
      return false;
    }
  }
  return false;
}

boolean signOut(){
  //connected 
  if(WiFi.status() == WL_CONNECTED){
    //send post request
    WiFiClient client; HTTPClient http;
    http.begin(client, signOutPath);
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(1024);
    doc["machineName"] = ID;
    doc["machineSecret"] = SECRET;
    doc["studentPIN"] = pass;
    doc["IP"] = WiFi.localIP();
    String msg; serializeJson(doc,msg);
    int httpResponseCode = http.POST(msg);
    if (httpResponseCode == 200) {
      return true;
    }
    else {
      lcd.clear();lcd.print("Error Signing Out: ");lcd.print(httpResponseCode);
      user = "";
      pass = "";
      http.end();
      return false;
    }
  }
  return false;
}
