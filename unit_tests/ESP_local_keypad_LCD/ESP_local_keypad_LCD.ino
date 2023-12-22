#include <Keypad.h>
#include <LiquidCrystal.h>

//Global
bool sim = true;
String pass = "";

void setup(){
  Serial.begin(9600);
}

void loop(){
  //handle keypress
  if (Serial.available() > 0){
    char key = tread();
    //D -> remove last char
    if (key=='D') {
      pass.remove(pass.length()-1);
      tclear();tprint(pass);
    }
    //* -> submit pass
    else if (key=='*') {
      // correct pass
      if (pass.length()==6) {
        tclear();tprint("Hello user");
      }
      //wrong pass
      else {
        tclear();tprint("Denied Access");
      }
      pass="";
    }
    //else -> add char to pass
    else {
      pass.concat(key);
      tclear();tprint(pass);
    }
  }
}

char tread(){
  return Serial.read();
}
void tprint(String x){
  Serial.print(x);
}
void tprint(char x){
  Serial.print(x);
}
void tprint(int x){
  Serial.print(x);
}
void tclear(){
  Serial.println();
}
