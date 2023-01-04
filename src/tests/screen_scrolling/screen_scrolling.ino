#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

void setup() {
  lcd.begin(16, 2);
}

void loop() {
  writeToScreenAndAutoScroll(0, "Distribuino v0");
  writeToScreenAndAutoScroll(1, "Veuillez entrer le code du produit souhaite");
}

void writeToScreenAndAutoScroll(int line, String text) {
  int textLength = text.length()+1;
  char str1[textLength];
  text.toCharArray(str1, textLength);
  
  for (int j = 0; j < max(1, textLength - 16); j++) {
    for(int i = 0; i < 16; i++){
      lcd.setCursor(i, line);
      if(i+j < textLength -1) lcd.write(str1[i+j]);
      else lcd.write(" ");
    }
    delay(500);
  }
}
