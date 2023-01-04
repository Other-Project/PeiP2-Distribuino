#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("Distribuino v0");
}

void loop() {
}
