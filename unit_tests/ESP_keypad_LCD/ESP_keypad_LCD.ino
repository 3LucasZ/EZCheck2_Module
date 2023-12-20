#include <Keypad.h>
#include <LiquidCrystal.h>

//Keypad
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte pin_rows[ROW_NUM] = {17, 5, 18, 19}; //connect to the row pinouts of the keypad {R1, R2, R3, R4}
byte pin_column[COLUMN_NUM] = {15, 2, 4, 16}; //connect to the column pinouts of the keypad {C1, C2, C3, C4}
Keypad pad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

//LCD
const int rs = 33, en = 25, d4 = 26, d5 = 27, d6 = 14, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Global
bool sim = true;
String pass = "";

void setup(){
  Serial.begin(9600);
  if (!sim){
    lcd.begin(16, 2);
    lcd.print("Welcome!");
  }
}

void loop(){
  //handle keypress
  char key = tread();
  if (key){
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
  if (!sim) return pad.getKey();
  else return Serial.read();
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
void tclear(){
  if (!sim) lcd.clear();
  else Serial.println();
}
