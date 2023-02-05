# Evan

## Séance 3 : 11 janvier 2023

### Branchement du clavier numérique

Un clavier numérique 4x4 nous avait été fourni lors de la première séance.  
Le clavier se branche avec 8 fils : 4 pour les colonnes, 4 pour les lignes. 
La combinaison des deux permettant de déterminer la touche enfoncée.

![IMG_9455](https://user-images.githubusercontent.com/24252743/211827163-4bb717a3-f87d-4973-9166-762b094d6374.jpg)

### Simple test du clavier

Pour récuprer la valeur de la touche enfoncé, j'ai utilisé la librairie [Keypad](https://playground.arduino.cc/Code/Keypad/)

Dans un premier temps, j'ai fait en sorte que la valeur de la touche enfoncé soit affichée dans le moniteur série

```cpp
#include <Keypad.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup(){
  Serial.begin(9600);
}

void loop(){
  char key = keypad.getKey();

  if (key){
    Serial.println(key);
  }
}
```

https://user-images.githubusercontent.com/24252743/211828880-aedc92f8-2ee3-4201-900f-da736e63023e.mp4


### Branchements plus poussés en utilisant le travail effectué lors des séances précédentes

J'ai souhaité que la saisit s'affiche directement à l'écran.  
Comme l'arduino Uno ne disposait pas de suffisament de PINs, j'ai récupéré une Arduino Mega 2560 qui m'a permis d'effectuer tout les branchements necessaires sur les PINs 30 à 35 pour l'écran et 46 à 53 pour le clavier numérique.  
Dans mon code, j'ai limitée la saisie à une taille de 4 caractères, ce qui est amplement suffisant pour notre projet.  
J'ai aussi permis que l'on puisse annuler la saisie en appuyant sur les touches `*` ou `#`

```cpp
#include <Keypad.h>
#include <LiquidCrystal.h>


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


const int codeLength = 4;
String productCode = "";

void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
}

void loop(){
  char key = keypad.getKey();

  if (key) {
    if(key == '*' | key == '#') productCode = ""; // Cancel
    else if(productCode.length() < codeLength) productCode += key;
  }
  
  writeToScreen(0, "Distribuino v0");
  if(productCode == "") writeToScreen(1, "Code produit ?");
  else writeToScreen(1, productCode);
}

void writeToScreen(int line, String text) {
  // Clear the screen line
  for(int i = 0; i < 16; i++){
    lcd.setCursor(i, line);
    lcd.write(" ");
  }
    
  lcd.setCursor(0, line);
  lcd.print(text);
}
```

![IMG_E9460](https://user-images.githubusercontent.com/24252743/211829593-24fa673d-84f6-402d-a8dd-75a28b4bf058.jpg)


En suite, j'ai branché un moteur pas à pas sur les PINs PWM tel que spécifié sur [le rapport de la séance 1](seance1-evan.md).  
Et j'ai fait en sorte que le moteur pas à pas effectue une rotation complète (360°) lorsque le code produit est entré (il n'y a, pour le moment, pas de vérification du code saisit, ni de demande de paiement)

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
```

![IMG_E9463](https://user-images.githubusercontent.com/24252743/211833150-fbe2c2fa-bd2f-41a4-bace-bef7a3c3b208.jpg)

https://user-images.githubusercontent.com/24252743/211832537-18a13e8d-21fa-4c46-9d62-8e0b3d8797a1.mp4
