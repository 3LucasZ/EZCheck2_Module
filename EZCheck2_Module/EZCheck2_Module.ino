#include <ArduinoJson.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <uri/UriBraces.h>

#include <Update.h>
#include <Preferences.h>

#include "env.h"
#include "pages.h"

//Webserver
WebServer webServer(80);
String signInPath;
String signOutPath;

//Global
bool sim = false;
bool signedIn = false;
String user = "";
String pass = "";
String apiKey = "";

//Preferences
Preferences preferences;
String id;
String safeId;
String tar;
String network;
String password;
bool isSTA;

//Keypad
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
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

void setup() {
  //activate
  Serial.begin(115200);
  if (!sim) lcd.begin(16, 2);
  preferences.begin("app", false);
  pinMode(red, OUTPUT); digitalWrite(red, LOW);
  pinMode(green, OUTPUT); digitalWrite(green, LOW);

  //load preferences
  id = preferences.getString("id", DEFAULT_ID); if (!id || id.length() <= 4) id = DEFAULT_ID;
  safeId = id; safeId.replace(" ", "-");
  tar = preferences.getString("tar", DEFAULT_TAR);
  signInPath = "https://" + tar + "/api/post/join-machine";
  signOutPath = "https://" + tar + "/api/post/leave-machine";
  network = preferences.getString("network", DEFAULT_NETWORK);
  password = preferences.getString("password", DEFAULT_PASSWORD);
  isSTA = preferences.getBool("isSTA", true);

  //begin wifi
  WiFi.setHostname(safeId.c_str());
  if (isSTA) {
    WiFi.begin(network.c_str(), password.c_str());
    tclear(); tprint("Connecting");
    int cnt = 0;
    while (WiFi.status() != WL_CONNECTED && cnt <= 22) {
      cnt++;
      if (cnt == 7) lcd.setCursor(0, 1);
      tprint(".");
      delay(1000);
    }
    if (WiFi.status() == WL_CONNECTED) {
      tclear();
      tprint("Online");
    }
    else {
      tclear();
      tprint("Offline");
    }
  } else {
    WiFi.softAP(id.c_str(), ADMIN_PASSWORD);
    tclear(); tprint("AP");
  }
  tprint(" V"); tprint(STAMP);
  lcd.setCursor(0, 1); tprint(id);
  MDNS.begin(safeId.c_str());

  //begin webserver
  createWebServerApi();
  webServer.begin();
  delay(1000);
}

void loop() {
  webServer.handleClient(); //handle webserver clients
  char key = tread(); //handle keypress
  if (key) {
    if (signedIn) signOut();
    if (key == 'D') { //D -> pop pass
      if (pass.length() > 0) pass.remove(pass.length() - 1);
      tclear(); tprintlong("PIN: " + hide(pass));
    } else if (key == '*') { //* -> submit pass
      if (pass == ADMIN_PASSWORD) { //toggle STA/AP
        preferences.putBool("isSTA", !isSTA);
        ESP.restart();
      } else signIn();
    } else { //all other keys -> add char to pass
      pass.concat(key);
      tclear(); tprintlong("PIN: " + hide(pass));
    }
  }
  delay(1);
}

boolean signIn() {
  //handle connected
  if (WiFi.status() == WL_CONNECTED) {
    //send request
    WiFiClientSecure client; client.setInsecure(); HTTPClient http;
    http.begin(client, signInPath);
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(1024);
    doc["machineName"] = id;
    doc["studentPIN"] = pass;
    doc["IP"] = WiFi.localIP();
    String msg; serializeJson(doc, msg);
    int resCode = http.POST(msg);
    //handle response
    String res = http.getString(); http.end();
    if (resCode == 200) {
      user = res; pass = ""; signedIn = true;
      tclear(); tprintlong(res); digitalWrite(green, HIGH); digitalWrite(red, LOW);
      return true;
    } else {
      user = ""; pass = ""; signedIn = false;
      tclear(); tprintlong(res); digitalWrite(red, HIGH);
      return false;
    }
  }
  //handle disconnected
  tclear(); tprint("Offline");
  return false;
}

boolean signOut() {
  //handle connected
  if (WiFi.status() == WL_CONNECTED) {
    //send request
    WiFiClientSecure client; client.setInsecure(); HTTPClient http;
    http.begin(client, signOutPath);
    http.addHeader("Content-Type", "application/json");
    DynamicJsonDocument doc(1024);
    doc["machineName"] = id;
    String msg; serializeJson(doc, msg);
    int resCode = http.POST(msg);
    //handle res
    String res = http.getString(); http.end();
    if (resCode == 200) {
      user = ""; pass = ""; signedIn = false;
      tclear(); tprintlong(res); digitalWrite(green, LOW);
      return true;
    }
    else {
      user = ""; pass = ""; signedIn = true;
      tclear(); tprintlong(res); digitalWrite(red, HIGH);
      return false;
    }
  }
  //handle disconnected
  tclear(); tprint("Offline");
  return false;
}

void createWebServerApi() {
  //PAGES
  webServer.on("/", HTTP_GET, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", indexHtml);
  });

  //API
  //preferences CRUD
  webServer.on("/api/reset/{}", HTTP_POST, []() {
    webServer.sendHeader("Connection", "close");
    String key = webServer.pathArg(0);
    if (key == ADMIN_PASSWORD) {
      webServer.send(200, "text/plain", "OK");
      preferences.clear();
      ESP.restart();
    } else {
      webServer.send(500, "text/plain", "BAD KEY");
    }
  });
  webServer.on(UriBraces("/api/setId/{}/{}"), HTTP_POST, []() {
    webServer.sendHeader("Connection", "close");
    String newId = webServer.pathArg(0);
    String key = webServer.pathArg(1);
    if (key == ADMIN_PASSWORD) {
      webServer.send(200, "text/plain", "OK");
      preferences.putString("id", urldecode(newId));
      ESP.restart();
    } else {
      webServer.send(500, "text/plain", "BAD KEY");
    }
  });
  webServer.on(UriBraces("/api/setTar/{}/{}"), HTTP_POST, []() {
    webServer.sendHeader("Connection", "close");
    String newTar = webServer.pathArg(0);
    String key = webServer.pathArg(1);
    if (key == ADMIN_PASSWORD) {
      webServer.send(200, "text/plain", "OK");
      preferences.putString("tar", newTar);
      ESP.restart();
    } else {
      webServer.send(500, "text/plain", "BAD KEY");
    }
  });
  webServer.on(UriBraces("/api/getInfo"), HTTP_GET, []() {
    webServer.sendHeader("Connection", "close");
    DynamicJsonDocument doc(2048);
    doc["id"] = id;
    doc["safeId"] = safeId;
    doc["tar"] = tar;
    doc["stamp"] = STAMP;
    doc["network"] = network;
    doc["defaultId"] = DEFAULT_ID;
    doc["signInPath"] = signInPath;
    doc["signOutPath"] = signOutPath;
    doc["IP"] = isSTA ? WiFi.localIP() : WiFi.softAPIP();
    String msg; serializeJson(doc, msg);
    webServer.send(200, "text/plain", msg);
  });
  //handling uploading firmware file
  webServer.on(UriBraces("/update/{}"), HTTP_POST, []() {
    webServer.sendHeader("Connection", "close");
    apiKey = webServer.pathArg(0);
    if (apiKey == ADMIN_PASSWORD) {
      webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    } else {
      webServer.send(500, "text/plain", "BAD KEY");
    }
  }, []() {
    HTTPUpload& upload = webServer.upload();
    if (apiKey == ADMIN_PASSWORD) {
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
        preferences.putBool("isSTA", true);
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
    }
  });
}

//io
char tread() {
  if (!sim) return pad.getKey();
  else if (Serial.available()) return Serial.read();
  else return '\0';
}
void tprintlong(String x) {
  if (!sim) {
    if (x.length() > 16) {
      lcd.print(x.substring(0, 16));
      lcd.setCursor(0, 1);
      lcd.print(x.substring(16));
    } else {
      lcd.print(x);
    }
  }
  Serial.print(x);
}
void tprint(String x) {
  if (!sim) lcd.print(x);
  Serial.print(x);
}
void tprint(char x) {
  if (!sim) lcd.print(x);
  Serial.print(x);
}
void tprint(int x) {
  if (!sim) lcd.print(x);
  Serial.print(x);
}
void tprint(IPAddress x) {
  if (!sim) lcd.print(x);
  Serial.print(x);
}
void tclear() {
  if (!sim) lcd.clear();
  Serial.println();
}
String hide(String str) {
  String ret = "";
  for (int i = 0; i < str.length(); i++)ret += "*";
  return ret;
}
unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}
String urldecode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {
      encodedString += c;
    }
    yield();
  }
  return encodedString;
}
