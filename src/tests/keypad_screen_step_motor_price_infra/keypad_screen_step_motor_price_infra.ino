#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Stepper.h>


struct Item {
  String itemCode;
  String itemName;
  float itemPrice;
};

struct Coin {
  int pin;
  float coinValue;
};

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

const int captors = 1;//5;
const Coin coinCaptors[] = {
  {41, 0.5},
  /*{42, 0.2},
  {43, 0.5},
  {44, 1.0},
  {45, 2.0}*/
};


Item items[] = {
  {"123A", "Bonbons", 2.5},
  {"456B", "Kitkat", 2.0},
  {"789C", "", 2.1},
  {"0000", "C'est un code pin", 3.0}
};


const int codeLength = 4;


void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  stepper1.setSpeed(rpm);

  for(int i = 0; i < captors; i++) pinMode(coinCaptors[i].pin, INPUT);
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

String waitForCode(){
  writeToScreen(0, "Distribuino v0.3");
  writeToScreen(1, "Code produit ?");

  String productCode = "";
  while(productCode.length() < codeLength) {
    char key = keypad.getKey();
    if (key) {
      if(key == '*' | key == '#') return waitForCode(); // Cancel
      else productCode += key;
      
      writeToScreen(1, productCode);
    }
  }
  return productCode;
}

Item* checkCode(String productCode) {
  for(int i = 0; i <= 4; i++) {
      if(i == 4)
      {
        writeToScreen(1, "Code invalide");
        delay(5000);
        productCode = "";
        return NULL;
      }
      else if(items[i].itemCode == productCode) {
        writeToScreen(0, items[i].itemName);
        writeToScreen(1, "Inserer " + String(items[i].itemPrice) + "e");
        return &items[i];
      }
    }
}

float waitForCoins(Item* item) {
  unsigned long dernierPassage; // Contiendra le temps écoulé depuis le début du programme
  bool detected[captors];
  float sum = 0;

  while(sum < item->itemPrice) {
    if(millis()-dernierPassage < 250) continue;
    for(int i = 0; i < captors; i++){
      
      // On teste si le capteur infrarouge détecte un passage 
      // On vérifie qu'aucun objet n'ait été détecté moins de 250 millisecondes plus tôt.
      if (digitalRead(coinCaptors[i].pin) == LOW && !detected[i]) {
        detected[i] = true;
        
        Serial.print("pièce de ");
        Serial.print(coinCaptors[i].coinValue);
        Serial.println(" détectée");
        
        sum += coinCaptors[i].coinValue; // On incrémente le nombre de passages détectés par le capteur

        float remaining = item->itemPrice - sum;
        if(remaining > 0) writeToScreen(1, "Inserer " + String(remaining) + "e");
      }
      else detected[i] = false;
    }
    dernierPassage = millis(); // On sauvegarde le temps écoulé entre le lancement du programme et ce passage
  }
  return -sum;
}



void loop() {
  String productCode = waitForCode();
  Item* item = checkCode(productCode);
  if (item == NULL) return;

  waitForCoins(item);
  writeToScreen(1, "Distribution...");
  stepper1.step(-stepsPerRevolution);
}
