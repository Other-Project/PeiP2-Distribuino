# Evan

## Séance 5 : 1 février 2023

### Travail au FabLab

> Voir le [compte rendu de Téo](seance5-teo.md) pour plus de détails sur les problèmes rencontrés

J'ai aidé Téo à enlever les supports de la partie basse de notre système de tri qui a été imprimé durant la semaine de ski.  
Malgrès tout nos efforts, la pièce reste innutilisable, les supports étant plus dure que la pièce elle-même.

En suite, nous avons essayé de corriger une erreure de cotation sur la rampe de tri, nous avons retiré la partie suppérieure de la rampe à la Dremel car celle-ci était trop proche de la rampe en elle-même. En retirant la partie suppérieure, le soutient des pièces avait en partie fondu, ce qui couplé avec une marge de seulement 1mm en bas des trous, ne permettait plus le passage des pièces. Nous avons alors poncé le support, mais en poncant, nous avons retirer plus de matière que necessaire et les pièces ne tombait plus dans les bons compartiments.

Nous nous somme résolus à refaire entièrement le système de tri, mais en limitant cette fois l'impression 3D au strict minimum, le reste sera fait en bois.

### Amélioration de la lisibilité du code

J'ai divisé le code en plusieurs fonctions afin d'en améliorer la lisibilité ainsi que pour en faciliter la modification dans le futur.
Afin de passer l'article séléctionné d'une fonction à une autre, j'ai utilisé un pointeur ce qui à l'avantage de pouvoir renvoyer une valeur `null` contrairement à un passage d'instance classique et qui permet un accès direct à l'article contrairement à l'index.


```cpp
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Stepper.h>


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

void waitForCoins(Item* item) {
  delay(5000);
}



void loop() {
  String productCode = waitForCode();
  Item* item = checkCode(productCode);
  if (item == NULL) return;

  waitForCoins(item);
  writeToScreen(1, "Distribution...");
  stepper1.step(-stepsPerRevolution);
}
```

### Détection des pièces

Après avoir nétoyé le code, je me suis attelé à la détection des pièces.
Il y aura un capteur pour chaque pièces (la valeur ayant été trié grace à une rampe qui fait tomber les pièces en fonction de leur taille).
Il me suffit donc de compter le nombre de passage devant chaque capteur pour déterminer la valeur des pièces incérées.
La fonction `waitForCoins` que j'ai prévu à cet effet, retourne un `float` qui contient la valeur à rendre à la fin de la transaction (dans le cas, où l'utilisateur a mis plus que nécessaire).

```cpp
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
```


https://user-images.githubusercontent.com/24252743/216056331-b4e8a50e-33cd-4e0c-b6ae-04efc0cd1f4b.mp4

![IMG_9496](https://user-images.githubusercontent.com/24252743/216057681-5ce1214f-2fe4-4f13-898a-f41817cd2d68.jpg)
