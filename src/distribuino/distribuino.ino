#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <AccelStepper.h>
#include <Servo.h>

struct Item
{
  String itemCode; // Le code à taper
  String itemName; // Le nom du produit
  float itemPrice; // Le prix du produit
  AccelStepper stepper; // Le moteur pas à pas correspondant
};

struct Coin
{
  float coinValue; // La valeur de la pièce
  int captorPin;   // Le numéro du pin sur lequel est branché le capteur infrarouge
  int servoPin;    // Le numéro du pin sur lequel est branché le servo-moteur
};


const int codeLength = 3; // La taille des codes produits

const int ROW_NUM = 4;    // Nombre de lignes du keypad
const int COLUMN_NUM = 4; // Nombre de colonne
char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte pin_rows[ROW_NUM] = {52, 50, 48, 46};      // Les numéros de pin des lignes
byte pin_column[COLUMN_NUM] = {51, 47, 49, 53}; // Les numéros de pin des colonnes
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
    {"B17", "Bonbons", 2.5, AccelStepper(AccelStepper::FULL4WIRE, 22, 26, 24, 28, false)},
    {"A12", "Colle", 2.3, AccelStepper(AccelStepper::FULL4WIRE, 32, 36, 30, 34, false)},
    {"C28", "Tictac", 2.1, AccelStepper(AccelStepper::FULL4WIRE, 25, 29, 23, 27, false)},
    {"A22", "Gauffre", 2.8, AccelStepper(AccelStepper::FULL4WIRE, 31, 35, 33, 37, false)}
};

void setup()
{
  Serial.begin(9600);

  // Initialisation de l'écran
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < itemNumber; i++){
    items[i].stepper.setMaxSpeed(300.0);
    items[i].stepper.setAcceleration(100.0);
  }

  // Initialisation des capteurs infrarouges et des servo-moteurs
  for (int i = 0; i < captors; i++)
  {
    pinMode(coinCaptors[i].captorPin, INPUT);
    servo[i].attach(coinCaptors[i].servoPin);
    servo[i].write(150);
  }

  Serial.println("Le programme a démarré");
}

void writeToScreen(int line, String text)
{
  // Clears the screen line
  for (int i = 0; i < 16; i++)
  {
    lcd.setCursor(i, line);
    lcd.print(" ");
  }

  // Writes the text
  lcd.setCursor(0, line);
  lcd.print(text);
}



void giveCoin(int i){
  servo[i].write(40);
  delay(375);
  servo[i].write(150);
  delay(750);
}

void getAllCoins(){
  for (int i = 0; i < captors; i++)
    for (int j = 0; j < 10; j++) giveCoin(i);
}

String waitForCode()
{
  writeToScreen(0, "Distribuino");
  writeToScreen(1, "Code produit ?");

  String productCode = "";
  while (productCode.length() < codeLength)
  {
    char key = keypad.getKey();
    if (key)
    {
      Serial.print("Saisie reçu: ");
      Serial.print(key);

      if (key == '*' || (key == '#' & productCode.length() <= 1))
      {
        Serial.println(", réinitialisation");
        return waitForCode(); // Cancel
      }
      else if (key == '#')
      {
        Serial.print(", suppression du dernier charactère (" + String(productCode.charAt(productCode.length() - 1)) + ")");
        productCode = productCode.substring(0, productCode.length() - 1);
      }
      else productCode += key;

      Serial.println(", saisie totale: " + String(productCode));
      writeToScreen(0, "Code produit ?");
      writeToScreen(1, productCode);
    }
  }
  return productCode;
}

Item* checkCode(String productCode) {
  if(productCode == "DDD") { getAllCoins(); return NULL;}
  
  Serial.print("Le code saisie (" + String(productCode));
  for (int i = 0; i <= itemNumber; i++)
  {
    if (i == itemNumber)
    {
      writeToScreen(1, "Code invalide");
      Serial.println(") est invalide");
      delay(3000);
      productCode = "";
      return NULL;
    }
    else if (items[i].itemCode == productCode)
    {
      Serial.println(") est valide (" + String(items[i].itemName) + ")");
      writeToScreen(0, items[i].itemName);
      writeToScreen(1, "Inserer " + String(items[i].itemPrice) + "e");
      return &items[i];
    }
  }
}

float waitForCoins(Item* item) {
  int last_state[captors];
  float sum = 0;

  while (sum < item->itemPrice)
  {
    char key = keypad.getKey();
    if (key == '*' | key == '#')
    {
      Serial.println("---- Annulation de la transaction ----");
      return sum == 0 ? -9999 : -sum; // Cancel
    }

    for (int i = 0; i < captors; i++)
    {
      int state = digitalRead(coinCaptors[i].captorPin);

      // On teste si le capteur infrarouge détecte un passage
      if (last_state[i] == HIGH && state == LOW)
      {
        Serial.println("pièce de " + String(coinCaptors[i].coinValue) + " détectée");
        sum += coinCaptors[i].coinValue;

        float remaining = item->itemPrice - sum;
        if(remaining > 0) writeToScreen(1, "Inserer " + String(remaining) + "e");
      }

      last_state[i] = state;
    }
  }

  float surplus = sum - item->itemPrice;
  Serial.println("Total " + String(sum) + "€ / " + String(item->itemPrice) + "€ demandés, exédant " + String(surplus) + "€");
  return surplus;
}

void giveMoneyBack(float amount)
{
  for (int i = captors - 1; i >= 0; i--)
  {
    float coinValue = coinCaptors[i].coinValue;
    int quantity = round(amount * 10) / int(coinValue * 10);
    float remaining = amount - coinValue * quantity;
    Serial.println("Reste à rembourser " + String(amount) + "€, rendu de " + String(quantity) + " pièces de " + String(coinValue) + "€, restant " + String(remaining) + "€");

    for (int j = 0; j < quantity; j++) giveCoin(i);

    amount = remaining;
  }
}

void loop()
{
  Serial.println("---- En attente de saisie ----");
  String productCode = waitForCode();
  Item* item = checkCode(productCode);
  if (item == NULL) return;

  Serial.println("En attente de paiement (" + String(item->itemPrice) + "€)");
  float surplus = waitForCoins(item);

  if (surplus >= 0)
  {
    Serial.println("Distribution de " + String(item->itemName));
    writeToScreen(1, "Distribution...");
    item->stepper.enableOutputs();
    item->stepper.move(stepsPerRevolution);
    item->stepper.runToPosition();
    item->stepper.disableOutputs();
  }
  else if(surplus == -9999) return; // Cancelled the transaction before paying anything
  else surplus *= -1; // Started to pay before cancelling

  writeToScreen(1, "Rendu monnaie");
  giveMoneyBack(surplus);

  writeToScreen(0, "Distribuino");
  writeToScreen(1, "Merci a bientot");
  Serial.println("---- Fin de la transaction ----");
  delay(3000);
}
