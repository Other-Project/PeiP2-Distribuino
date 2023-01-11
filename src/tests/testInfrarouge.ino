// Permet de compter le nombre de passages de la pièce devant le capteur infrarouge

const int INFRAROUGE1= 10;
int nombrePieces = 0;
unsigned long dernierPassage = 0;
unsigned long temps = 100; // Contiendra le temps écoulé depuis le début du programme

void setup(){
  pinMode(INFRAROUGE1,INPUT);
  Serial.begin(9600); // vitesse de communication avec l'arduino
}

void loop(){
  // On teste si le capteur infrarouge détecte un passage 
  // et on s'assure qu'aucun objet n'ait été détecté moins de 100millisecondes plus tôt.
  // Cela permet d'éviter qu'une bille passant à faible vitesse devant le capteur comptabilise plusieurs passages
  if (digitalRead(INFRAROUGE1)==LOW && millis()-dernierPassage>=100){ 
    Serial.println("pièce détectée");
    nombrePieces+=1; // On incrémente le nombre de passages détectés par le capteur
    dernierPassage = millis(); // Et on sauvegarde le temps écoulé entre le lancement du programme et ce passage
    Serial.print("Nombre de pièces :"); // Finalement on affiche le nombre de passages sur le moniteur série
    Serial.println(nombrePieces);
  }
}
