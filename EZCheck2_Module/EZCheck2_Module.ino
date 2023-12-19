#include <ArduinoJson.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HttpClient.h>
#include "env.h"

//constants
const String server = "http://ezserver.local";
const String join_path = "/post/join-machine";
const String leave_path = "/post/leave-machine";

//global state
bool signedIn = false;
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

//LED
const int red  =  22;
const int green = 23; 

void setup(){
  //activate
  Serial.begin(115200);
  WiFi.begin(NETWORK, PASSWORD);
  lcd.begin(16, 2);
  pinMode(red,OUTPUT); 
  pinMode(green,OUTPUT); 
  
  //setup wifi
  lcd.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }
  lcd.clear();
  lcd.print("Connected");
  delay(1000);
  
  //init state
  lcd.clear(); lcd.print("Welcome!");
  digitalWrite(red,LOW);
  digitalWrite(green,LOW);
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
          lcd.clear();
          String message = "Hello ";
          message.concat(curr_name);
          lcd.print(message);
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
  if(WiFi.status() == WL_CONNECTED){
    //confirmation
    lcd.clear(); lcd.print("Logging in...");
    //send post request
    WiFiClient client; HTTPClient http;
    http.begin(client, path);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"code\":\"" + pass + "\",\"machineID\":\"" + machineID+"\"}");
    //valid request
    String response = http.getString();
    DynamicJsonDocument response_data(1024);
    deserializeJson(response_data, response);
    bool auth = response_data["authorized"];
    
    if (auth) {
      String temp_name = response_data["name"];
      curr_name = temp_name;
      http.end();
      pass = "";
      return true;
    }
    else {
      lcd.clear();
      lcd.print("Error: ");
      lcd.print(httpResponseCode);
      Serial.println(httpResponseCode);
      curr_name = "";
      http.end();
      pass = "";
      return false;
    }
    // Free resources
  }
  return false;
}

boolean signOut(){
  //connected 
  if(WiFi.status() == WL_CONNECTED){
    //send post request
    WiFiClient client;
    HTTPClient http;
    http.begin(client, signout);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"name\":\"" + curr_name + "\",\"machineID\":\"" + machineID+"\"}");
    
    if (httpResponseCode == 200) {
      return true;
    }
    else {
      lcd.clear();
      lcd.print("Error Signing Out: ");
      lcd.print(httpResponseCode);
      Serial.println(httpResponseCode);
      curr_name = "";
      http.end();
      pass = "";
      return false;
    }
    // Free resources
  }
  return false;
}
