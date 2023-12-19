#include <Keypad.h>

const int ROW_NUM = 4; //4 rows
const int COLUMN_NUM = 4; //4 columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {16, 4, 2, 15};
byte pin_column[COLUMN_NUM] = {19, 18, 5, 17};

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const int len = 6;
String pass = "";

void setup(){
  Serial.begin(115200);
  Serial.println("type your password in the keypad, and it will show below:");
}

void loop(){
  char key = keypad.getKey();
  if (key){
    if (key=='D'){
      Serial.println("passcode was reset");
      pass="";
    } else {
      pass.concat(key);
      if (pass.length()==6) {
        Serial.println("The passcode you entered was: "+pass);
        pass="";
      } else {
        Serial.println(pass);
      }
    }
  }
}
