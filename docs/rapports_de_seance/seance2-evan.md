# Evan

## Séance 2 : 4 janvier 2023

### Branchement de l'écran

J'ai récuperé un écran LCD 16x2 (modèle *1602A*) sur le précédant projet de distributeur

J'ai effectué les branchements suivants :

| Pin sur l'Arduino | Pin sur l'écran | Pin sur le potentiomètre 10K |
|:-----------------:|:---------------:|:----------------------------:|
|         5V        |     VDD<br>A    |                              |
|        GND        | VSS<br>RW<br>K  |            Gauche            |
|                   |        V0       |            Centre            |
|         7         |        RS       |                              |
|         8         |        E        |                              |
|         9         |        D4       |                              |
|         10        |        D5       |                              |
|         11        |        D6       |                              |
|         12        |        D7       |                              |

![IMG_9445](https://user-images.githubusercontent.com/24252743/210718463-cb0f7cc4-8dc7-4cb1-b210-fed97feccb25.jpeg)

Le potentiomètre permet de régler le contraste de l'écran.

https://user-images.githubusercontent.com/24252743/210729041-f911d8b1-f771-4000-8fe1-7024fb148205.mp4


### Premiers tests d'affichage

J'ai tout d'abord souhaité écrire un simple texte sur l'écran, pour celà, j'ai utilisé la bibliothèque [LiquidCrystal](https://www.arduino.cc/reference/en/libraries/liquidcrystal/)

```cpp
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("Distribuino v0");
}

void loop() {
}
```

Puis, j'ai ajouté un effet de scrolling du texte pour pouvoir afficher de plus longues phrases

```cpp
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
```

https://user-images.githubusercontent.com/24252743/210729169-198bbead-360d-43c5-a217-ae5832e5f0bd.mp4


Constatant que l'écran "sintillait" lors de la réécriture du texte, ce qui est assez génant à la lecture, j'ai modifié mon programme pour afficher sur une meme ligne plusieurs bouts de phrases alternativements.

```cpp
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
```

https://user-images.githubusercontent.com/24252743/210720415-fd54bd8f-45fb-428e-b372-571b7d39c32a.mp4
