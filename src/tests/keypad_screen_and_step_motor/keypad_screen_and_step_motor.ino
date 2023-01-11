#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Stepper.h>


const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte pin_rows[ROW_NUM] = {47, 49, 51, 53}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {46, 48, 50, 52}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);


LiquidCrystal lcd(31, 33, 35, 30, 32, 34);


const int stepsPerRevolution = 2048;
const int rpm = 16;
Stepper stepper1 = Stepper(stepsPerRevolution, 8, 10, 9, 11);



const int codeLength = 4;
String productCode = "";

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
  stepper1.setSpeed(rpm);
}

void loop(){
  char key = keypad.getKey();

  if (key) {
    if(key == '*' | key == '#') productCode = ""; // Cancel
    else if(productCode.length() < codeLength) productCode += key;
  }
  
  writeToScreen(0, "Distribuino v0.3");
  if(productCode == "") writeToScreen(1, "Code produit ?");
  else if(productCode.length() == codeLength) 
  {
    writeToScreen(1, "Distribution...");
    stepper1.step(-stepsPerRevolution);
    productCode = "";
  }
  else writeToScreen(1, productCode);
}

void writeToScreen(int line, String text) {
  // Clears the screen line
  for(int i = 0; i < 16; i++){
    lcd.setCursor(i, line);
    lcd.write(" ");
  }

  // Writes the text
  lcd.setCursor(0, line);
  lcd.print(text);
}
