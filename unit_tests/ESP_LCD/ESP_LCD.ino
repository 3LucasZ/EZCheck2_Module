#include <LiquidCrystal.h>

const int rs = 32, en = 33, d4 = 25, d5 = 26, d6 = 27, d7 = 14;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2); // set up LCD # of columns and rows
  lcd.print("hello world!");
}

void loop() {
  lcd.setCursor(0, 1); // set cursor to column 0, line 1
  lcd.print(millis()/1000); // print # of seconds since last reset
  Serial.println(millis()/1000);
}
