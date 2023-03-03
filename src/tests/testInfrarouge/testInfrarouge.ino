// Permet de compter le nombre de passages de la pièce devant le capteur infrarouge

const int INFRAROUGE1= 6;
unsigned long dernierPassage = 0; // Contiendra le temps écoulé depuis le début du programme
int nombrePieces = 0;

void setup(){
  pinMode(INFRAROUGE1,INPUT);
  Serial.begin(9600); // vitesse de communication avec l'arduino

  Serial.println("Debut");
}

void loop() {
  // On teste si le capteur infrarouge détecte un passage 
  // On vérifie qu'aucun objet n'ait été détecté moins de 250 millisecondes plus tôt.
  if (pulseIn(INFRAROUGE1, LOW) && millis()-dernierPassage>=250) {
    nombrePieces += 1; // On incrémente le nombre de passages détectés par le capteur
    dernierPassage = millis(); // On sauvegarde le temps écoulé entre le lancement du programme et ce passage
    
    Serial.print("pièce détectée, nombre de pièces : "); // On affiche le nombre de passages sur le moniteur série
    Serial.println(nombrePieces);
  }
}
