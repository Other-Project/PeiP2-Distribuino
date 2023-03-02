#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <Servo.h>


struct Coin {
  float coinValue; // La valeur de la pièce
  int captorPin; // Le numéro du pin sur lequel est branché le capteur infrarouge
  int servoPin; // Le numéro du pin sur lequel est branché le servo-moteur
};

LiquidCrystal_I2C lcd(0x27, 16, 2); // Ecran LCD branché à l'adresse 0x27 (trouvable grace au programme fourni) pouvant afficher 16 char par ligne et disposant de 2 lignes

const int captors = 2; // Nombre de capteurs
const Coin coinCaptors[] = {
  {0.1, 2, 9},
  {0.2, 3, 10},
  {0.5, 5, 12},
  {1.0, 4, 11},
  {2.0, 6, 13}
};
Servo servo[captors];

void setup() {
  Serial.begin(9600);

  // Initialisation de l'écran
  lcd.init();
  lcd.backlight();
  
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

float waitForCoins(float price) {
  unsigned long dernierPassage; // Contiendra le temps écoulé depuis le début du programme
  bool detected[captors];
  float sum = 0;

  while(sum < price) {
    
    //if(millis()-dernierPassage < 25) continue;
    for(int i = 0; i < captors; i++){
      
      // On teste si le capteur infrarouge détecte un passage 
      // On vérifie qu'aucun objet n'ait été détecté moins de 250 millisecondes plus tôt.
      if (digitalRead(coinCaptors[i].captorPin) == LOW && !detected[i]) {
        detected[i] = true;
        
        Serial.print("pièce de ");
        Serial.print(coinCaptors[i].coinValue);
        Serial.println(" détectée");
        
        sum += coinCaptors[i].coinValue; // On incrémente le nombre de passages détectés par le capteur

        float remaining = price - sum;
        if(remaining > 0) writeToScreen(1, "Inserer " + String(remaining) + "e");
      }
      else detected[i] = false;
    }
    dernierPassage = millis(); // On sauvegarde le temps écoulé entre le lancement du programme et ce passage
  }

  float surplus = sum - price;
  Serial.print("Total ");
  Serial.print(sum);
  Serial.print("€ / ");
  Serial.print(price);
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

    for(int j = 0; j < quantity; j++){
      servo[i].write(40);
      delay(375);
      servo[i].write(150);
      delay(750);
    }

    amount = remaining;
  }
}


void loop() {
  float surplus = waitForCoins(0.02) + 0.02;
  waitForCoins(surplus);
  delay(500);
  giveMoneyBack(surplus);
  giveMoneyBack(surplus);
}
