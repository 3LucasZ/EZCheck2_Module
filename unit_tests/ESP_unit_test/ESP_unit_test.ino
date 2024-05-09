#include <Keypad.h>

#include <LiquidCrystal.h>

//Keypad vars
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {16, 4, 2, 15};
byte pin_column[COLUMN_NUM] = {19, 18, 5, 17};

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

//LCD vars
const int rs = 32, en = 33, d4 = 25, d5 = 26, d6 = 27, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Gasher
const int red = 22;
const int green = 23;

//Pass vars
String pass = "";

void setup(){
  Serial.begin(9600);
  //Initial state
  lcd.begin(16, 2);
  lcd.print("Welcome!");
  pinMode(red,OUTPUT); 
  digitalWrite(red,HIGH);
  pinMode(green,OUTPUT); 
  digitalWrite(green,LOW);
}

void loop(){
  //handle keypress
  char key = keypad.getKey();
  if (key){
    //D -> remove last char
    if (key=='D') {
      pass.remove(pass.length()-1);
      //display
      lcd.clear();
      lcd.print(pass);
    }
    //* -> submit pass
    else if (key=='*') {
      lcd.clear();
      // correct pass
      if (pass.length()==6) {
        lcd.print("Hello Mr. Huber");
        digitalWrite(green,HIGH);
        digitalWrite(red,LOW);
      }
      //wrong pass
      else {
        lcd.print("Denied Access");
        digitalWrite(green,LOW);
        digitalWrite(red,HIGH);
      }
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
