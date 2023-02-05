# Evan

## Séance 4 : 18 janvier 2023

### Présentation orale du projet

Avec Téo, nous avons présenté notre projet durant 5 minutes à un jury composé de M. Charlon et de M. Peter puis, tous deux nous on posé quelques questions et on émit des recommandations pour la suite du projet.

### Gestion des articles

J'ai implémenté un système de gestion des articles capable de connaitre le prix des articles ainsi que de vérifier la validé du code entré

Dans un premier temps, j'ai utilisé la librairie [Dictionary](https://www.arduino.cc/reference/en/libraries/dictionary/) qui m'a permis d'associer un prix à un code article

```cpp
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

Dictionary &d = *(new Dictionary(4));

const int codeLength = 4;
String productCode = "";

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
  stepper1.setSpeed(rpm);
  d("123A", 2.5);
  d("456B", 2.0);
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
    if(d(productCode)) {
        writeToScreen(1, "Inserer " + String(d[productCode]) + "e");
        delay(5000);
        writeToScreen(1, "Distribution...");
        stepper1.step(-stepsPerRevolution);
        productCode = "";
    }
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
```

https://user-images.githubusercontent.com/24252743/213459873-f7ff2302-5194-409e-a2dc-9b17383bf209.mp4


En suite, j'ai souhaité pouvoir afficher le nom de l'article sélectionné, j'ai donc créé une structure afin de faciliter l'accès aux données
```cpp
struct Item {
  String itemCode;
  String itemName;
  float itemPrice;
};
```

Mais je me suis rendu compte que la librairie que j'utilisais pour le dictionnaire ne les supportait pas.  
J'ai donc opté pour utiliser une simple liste dans laquelle le programme va itérer à la recherche de l'article

```cpp
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Stepper.h>
#include <Dictionary.h>


struct Item {
  String itemCode;
  String itemName;
  float itemPrice;
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


Item items[] = {
  {"123A", "Bonbons", 2.5},
  {"456B", "Kitkat", 2.0},
  {"789C", "", 2.1},
  {"0000", "C'est un code pin", 3.0}
};


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
    for(int i = 0; i <= 4; i++){
      if(i == 4)
      {
        writeToScreen(1, "Code invalide");
        delay(5000);
        productCode = "";
      }
      else if(items[i].itemCode == productCode) {
        writeToScreen(0, items[i].itemName);
        writeToScreen(1, "Inserer " + String(items[i].itemPrice) + "e");
        delay(5000);
        writeToScreen(1, "Distribution...");
        stepper1.step(-stepsPerRevolution);
        productCode = "";
        break;
      } 
    }
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
```

https://user-images.githubusercontent.com/24252743/213457605-0a6aa33a-0515-48f7-81d4-5080b71538fd.mp4
