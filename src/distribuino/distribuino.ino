#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Stepper.h>
#include <Servo.h>


struct Item {
  String itemCode;
  String itemName;
  float itemPrice;
};

struct Coin {
  float coinValue;
  int captorPin;
  int servoPin;
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


const int stepsPerRevolution = 2038;
const int rpm = 16;
Stepper stepper1 = Stepper(stepsPerRevolution, 9, 11, 8, 10);

const int captors = 5;
const Coin coinCaptors[] = {
  {0.1, 41, 2},
  {0.2, 42, 3},
  {0.5, 43, 4},
  {1.0, 44, 5},
  {2.0, 45, 6}
};
Servo servo[captors];


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

  for(int i = 0; i < captors; i++) {
    pinMode(coinCaptors[i].captorPin, INPUT);
    servo[i].attach(coinCaptors[i].servoPin);
  }

  Serial.println("Le programme a démarré");
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
  writeToScreen(0, "Distribuino");
  writeToScreen(1, "Code produit ?");

  String productCode = "";
  while(productCode.length() < codeLength) {
    char key = keypad.getKey();
    if (key) {
      
      Serial.print("Saisie reçu: ");
      Serial.print(key);
      
      if(key == '*' | key == '#') {
        Serial.println(", réinitialisation");
        return waitForCode(); // Cancel
      }
      else productCode += key;
      
      Serial.print(", saisie totale: ");
      Serial.println(productCode);
      writeToScreen(1, productCode);
    }
  }
  return productCode;
}

Item* checkCode(String productCode) {
  Serial.print("Le code saisie (");
  Serial.print(productCode);
  for(int i = 0; i <= 4; i++) {
      if(i == 4)
      {
        writeToScreen(1, "Code invalide");
        Serial.println(") est invalide");
        delay(3000);
        productCode = "";
        return NULL;
      }
      else if(items[i].itemCode == productCode) {
        Serial.print(") est valide (");
        Serial.print(items[i].itemName);
        Serial.println(")");
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
    if(millis()-dernierPassage < 50) continue;
    for(int i = 0; i < captors; i++){
      
      // On teste si le capteur infrarouge détecte un passage 
      // On vérifie qu'aucun objet n'ait été détecté moins de 250 millisecondes plus tôt.
      if (digitalRead(coinCaptors[i].captorPin) == LOW && !detected[i]) {
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

  float surplus = sum - item->itemPrice;
  Serial.print("Total ");
  Serial.print(sum);
  Serial.print("€ / ");
  Serial.print(item->itemPrice);
  Serial.print("€ demandés, exédant ");
  Serial.print(surplus);
  Serial.println("€");
  return surplus;
}


void giveMoneyBack(float amount){
  for(int i = captors - 1; i >= 0; i--){
    float coinValue = coinCaptors[i].coinValue;
    int quantity = int(amount / coinValue);
    float remaining = amount - coinValue * quantity;

    Serial.print("Reste à rembourser ");
    Serial.print(amount);
    Serial.print("€, rendu de ");
    Serial.print(quantity);
    Serial.print(" pièces de ");
    Serial.print(coinValue);
    Serial.print("€, restant ");
    Serial.print(remaining);
    Serial.println("€");

    servo[i].write(140);
    delay(100);
    servo[i].write(0);
    delay(100);

    amount = remaining;
  }
}



void loop() {
  Serial.println("---- En attente de saisie ----");
  String productCode = waitForCode();
  Item* item = checkCode(productCode);
  if (item == NULL) return;

  Serial.print("En attente de paiement (");
  Serial.print(item->itemPrice);
  Serial.println("€)");
  float surplus = waitForCoins(item);
  
  Serial.print("Distribution de ");
  Serial.println(item->itemName);
  writeToScreen(1, "Distribution...");
  stepper1.step(stepsPerRevolution);

  writeToScreen(1, "Rendu monnaie");
  giveMoneyBack(surplus);

  writeToScreen(1, "Merci a bientot");
  Serial.println("---- Fin de la transaction ----");
  delay(3000);
}
