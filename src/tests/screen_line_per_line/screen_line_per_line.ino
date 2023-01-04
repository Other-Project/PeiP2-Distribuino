#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

void setup() {
  lcd.begin(16, 2);
}

void loop() {
  String line0[] = {"Distribuino v0"};
  String line1[] = {"Veuillez entrer", "le code du", "produit souhaite"};
  writeToScreenAndAutoScroll(0, line0, 1);
  writeToScreenAndAutoScroll(1, line1, 3);
}

void writeToScreenAndAutoScroll(int line, String lines[], int lineNumber) {
  for(int j = 0; j < lineNumber; j++){
    String text = lines[j];
    int textLength = text.length()+1;
    char str1[textLength];
    text.toCharArray(str1, textLength);
  
    for(int i = 0; i < 16; i++){
      lcd.setCursor(i, line);
      lcd.write(" ");
    }
    
    lcd.setCursor(0, line);
    lcd.write(str1);
    delay(1500);
  }
}
