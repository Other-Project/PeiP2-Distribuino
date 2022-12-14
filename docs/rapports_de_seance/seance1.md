# Séance 1 : 14 décembre 2022

## Evan

### Démontage d'un précédent projet de distributeur automatique
Récupération des moteurs pas à pas, du système de tri des pièces et des divers autres composants

![Précédant projet de distributeur](assets/seance1/precedant_projet_de_distributeur.jpg)
![Interieur de l'ancien projet](assets/seance1/interieur_ancien_projet.jpg)
![Composants récupérés](assets/seance1/composants_recuperes.jpg)
![Écran](assets/seance1/ecran.jpg)


### Tests de fonctionnement des moteurs pas à pas
A partir des branchements de l'ancien projet, j'ai pu isoler la partie dédiée au fonctionnement des moteurs pas à pas.  
Celle-ci utilise un `ULN2003APG` pour gérer les controles.  
J'ai utilisé la librairie [Stepper](https://docs.arduino.cc/learn/electronics/stepper-motors) pour communiquer avec la puce qui controle le moteur pas à pas.  

Exemple de code :

```c
#include <Stepper.h>
const int stepsPerRevolution = 2048;
const int rpm = 12;
Stepper stepper1 = Stepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  stepper1.setSpeed(rpm);
}

void loop() {
  stepper1.step(-stepsPerRevolution);
}
```

![Moteur pas à pas](assets/seance1/moteur_pas_a_pas.jpg)
![Branchements ULN2003APG](assets/seance1/branchements_ULN2003APG.jpg)
![Moteur pas à pas en fonctionnement](assets/seance1/moteur_pas_a_pas_en_fonctionnement.mp4)


## Téo
