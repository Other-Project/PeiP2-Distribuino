#  Séance 3 Téo Baillot d'Estivaux:  

Aujourd'hui j'ai continué la modélisation du système de tri de pièces. J'ai découvert en ouvrant ma modélisation que certaines esquisses s'étaient décallées et que la modélisation n'était pas dans l'état dans lequel je l'avais laissée peut-être parce que je l'avais mal enregistrée.
J'ai donc perdu pas mal de temps à tout remettre correctement et j'ai ensuite commencé à réfléchir à comment j'allais placer les capteurs sur les tubes. Finalement un capteur infrarouge sera placé en haut de chaque tube de façon à détecter lorsqu'une pièce entre et lorsque le tube est plein.  

Il y avait quand même le risque que l'épaisseur de la pièce soit trop faible et empêche le capteur de la détecter. J'ai donc préféré tester le fonctionnement du capteur afin de m'assurer qu'il détectera bien la pièce lorsqu'elle passera devant avant de faire cette partie de la modélisation.  

Pour finir j'ai commencé la modélisation de la plateforme qui sera en dessous des tubes à 1.5 fois l'épaisseur de la pièce en question de distance et qui accueillera le servo-moteur qui permettra de faire sortir les pièces une par une du tube pour rendre la monnaie.  

J'avais prévu de faire la modélisation du système de tri des pièces en 3 séances, donc je n'ai pas réussi à respecter ce délai, mais j'ai tout de même bien avancé car j'ai déjà fait le fonctionnement des capteurs infrarouges.
A la prochaine séance, il ne restera plus qu'à finir le peu qu'il reste à faire sur les plateformes pour pouvoir y insérer le servo-moteur et à faire la rampe qui accompagnera les pièces à la sortie de la machine.  

L'objectif est donc de lancer l'impression du système de tri de pièce et de coder le rendu de la monnaie grâce aux servo-moteurs lors de la prochaine séance.

## Le fonctionnement du capteur infrarouge:
Le principe de fonctionnement du capteur est le suivant: le capteur est équipé d'un émetteur de lumière infrarouge et d'un récepteur qui détecte l'intensité de la réflexion lumineuse. Ainsi si un obstacle se trouve dans le champ de vision du capteur, la radiation sera réfléchie par l'obstacle et le récepteur la mesurera. Le potentiomètre (carré bleu) nous permet de régler cette distance.  

![image](https://user-images.githubusercontent.com/79744829/211818236-2b4bf64e-88ce-4eb5-b5ee-c8502acbbc98.png)  

J'ai choisi de prendre un capteur qui se fixera à la verticale au tube et qui ne nécéssite qu'un trou car c'est ce qui est le plus adapté à nos besoins.  

Pour le moment le programme que j'ai fait est assez simple et a juste permis de détecter si une pièce passait devant et de compter le nombre de pièces qui est passé devant.  

```cpp
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
  // On vérifie qu'aucun objet n'ait été détecté moins de 100millisecondes plus tôt.
  if (digitalRead(INFRAROUGE1)==LOW && millis()-dernierPassage>=100){ 
    Serial.println("pièce détectée");
    nombrePieces+=1; // On incrémente le nombre de passages détectés par le capteur
    dernierPassage = millis(); // On sauvegarde le temps écoulé entre le lancement du programme et ce passage
    Serial.print("Nombre de pièces :"); // On affiche le nombre de passages sur le moniteur série
    Serial.println(nombrePieces);
  }
}
``` 

![image](https://user-images.githubusercontent.com/79744829/211818491-589a1d76-98bc-45a1-b73f-ec865e528ff9.png)  

https://user-images.githubusercontent.com/24252743/212019928-68504ad0-9cb5-4a2b-b036-74bf6e9d8f75.mp4

https://user-images.githubusercontent.com/24252743/212019966-c19988e2-a1df-4e1a-bad1-67831ce1d69a.mp4



## La partie modélisation:  

J'ai fait un trou sur chaque tube qui acceuillera le capteur, il faudra peut-être faire quelques modification de cotes car il ne faudrait pas que le capteur dépasse à l'intérieur du tube ce qui pourrait géner le passage des pièces.  

![image](https://user-images.githubusercontent.com/79744829/211818919-d1e62945-4c36-423b-9217-3fb98299446b.png)

J'ai aussi fait simplement ces plateformes, la partie de droite accueillera les pièces et celle de gauche le servo-moteur qui permettra de rendre la monnaie.  

![image](https://user-images.githubusercontent.com/79744829/211819037-cbc6743c-c057-412f-94c8-5b277e259153.png)
![image](https://user-images.githubusercontent.com/79744829/211819264-3c017459-9d10-47b0-b0f2-821325bc3b2c.png)

