#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <Servo.h>


struct Item {
  String itemCode; // Le code à taper
  String itemName; // Le nom du produit
  float itemPrice; // Le prix du produit
  Stepper stepper; // Le moteur pas à pas correspondant
};

struct Coin {
  float coinValue; // La valeur de la pièce
  int captorPin; // Le numéro du pin sur lequel est branché le capteur infrarouge
  int servoPin; // Le numéro du pin sur lequel est branché le servo-moteur
};


const int codeLength = 4; // La taille des codes produits

const int ROW_NUM = 4; // Nombre de lignes du keypad
const int COLUMN_NUM = 4; // Nombre de colonne
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte pin_rows[ROW_NUM] = {47, 49, 51, 53}; // Les numéros de pin des lignes
byte pin_column[COLUMN_NUM] = {46, 48, 50, 52}; // Les numéros de pin des colonnes
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);


LiquidCrystal_I2C lcd(0x27, 16, 2); // Ecran LCD branché à l'adresse 0x27 (trouvable grace au programme fourni) pouvant afficher 16 char par ligne et disposant de 2 lignes


const int captors = 5; // Nombre de capteurs
const Coin coinCaptors[] = {
  {0.1, 2, 9},
  {0.2, 3, 10},
  {0.5, 5, 12},
  {1.0, 4, 11},
  {2.0, 6, 13}
};
Servo servo[captors];


const int stepsPerRevolution = 2038;
const int rpm = 16;

const int itemNumber = 4; // Nombre d'articles différents en vente
Item items[] = {
  {"123A", "Bonbons", 2.5, Stepper(stepsPerRevolution, 24,28,22,26)},
  {"456B", "Kitkat", 2.0, Stepper(stepsPerRevolution, 32, 36, 30,34)},
  {"789C", "Tictac", 2.1, Stepper(stepsPerRevolution, 25,29,23,27)},
  {"0000", "C'est un code pin", 3.0, Stepper(stepsPerRevolution, 33, 37, 31,35)}
};


void setup() {
  Serial.begin(9600);

  // Initialisation de l'écran
  lcd.init();
  lcd.backlight();

  for(int i = 0; i < itemNumber; i++) items[i].stepper.setSpeed(rpm); // On défini la vitesse de rotation des moteurs pas à pas

  // Initialisation des capteurs infrarouges et des servo-moteurs
  for(int i = 0; i < captors; i++) {
    pinMode(coinCaptors[i].captorPin, INPUT);
    servo[i].attach(coinCaptors[i].servoPin);
    servo[i].write(150);
  }

  Serial.println("Le programme a démarré");
}

void writeToScreen(int line, String text) {
  // Clears the screen line
  for(int i = 0; i < 16; i++){
    lcd.setCursor(i, line);
    lcd.print(" ");
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
  for(int i = 0; i <= itemNumber; i++) {
      if(i == itemNumber)
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
  int last_state[captors];
  float sum = 0;

  while(sum < item->itemPrice) {
	char key = keypad.getKey();
   if(key == '*' | key == '#') {
      Serial.println("---- Annulation de la transaction ----");
      return -sum; // Cancel
   }
		
    for(int i = 0; i < captors; i++){
      int state = digitalRead(coinCaptors[i].captorPin);

      // On teste si le capteur infrarouge détecte un passage
      if (last_state[i] == HIGH && state == LOW) {
        
        Serial.print("pièce de ");
        Serial.print(coinCaptors[i].coinValue);
        Serial.println(" détectée");
        
        sum += coinCaptors[i].coinValue;
		  
        float remaining = item->itemPrice - sum;
        if(remaining > 0) writeToScreen(1, "Inserer " + String(remaining) + "e");
      }
      
      last_state[i] = state;
    }
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


void giveMoneyBack(float amount) {
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

    for(int j = 0; j < quantity; j++) {
      servo[i].write(40);
      delay(375);
      servo[i].write(150);
      delay(750);
    }

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

  if(surplus >= 0){
    Serial.print("Distribution de ");
    Serial.println(item->itemName);
    writeToScreen(1, "Distribution...");
    item->stepper.step(stepsPerRevolution);
  }
  else surplus *= -1;

  writeToScreen(1, "Rendu monnaie");
  giveMoneyBack(surplus);

  writeToScreen(1, "Merci a bientot");
  Serial.println("---- Fin de la transaction ----");
  delay(3000);
}
