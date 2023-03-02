# Evan

## Séance 8 : 1 mars 2023

### Fin de la construction des cases

J'ai ajouté le fond sur lequel j'ai fait découper des trous pour les moteurs pas à pas.  
Comme j'avais oublié de faire une encoche pour passer les câbles, j'ai découpé de petites encoches en bas de chaque trous.  
Puis j'ai visser avec des boulons les moteurs pas à pas. 
Malheureusement, il n'y avait pas assez de boulons, je vais devoir en acheter pour visser les moteurs restants.

### Calibrage des capteurs infra-rouge

J'ai essayer de faire fonctionner les capteurs infra-rouge devant capter le passage d'une pièce dans le tube.
Outre certains capteurs defectueux, j'ai remarqué que les capteurs étaient trop éloigné du tube et produisaient beaucoup de faux-positifs.
Téo a donc réduit l'écartement des capteurs à la dremel, ce qui a permi une nette amélioration de la detection.
En revanche, le calibrage n'est toujours pas parfait et la detection ne se fait pas toujours correctement.
Les potentiomètres sont pas assez precis pour un calibrage efficace, comme les capteurs disposent d'une sortie analogique en plus d'une sortie digitale, j'ai la possibilité si necessaire de faire le calibrage dans le code.

https://user-images.githubusercontent.com/24252743/222414979-ab9823aa-37fc-4148-a2f4-29b1150eda77.mp4

### Tests des servo-moteurs servant au rendu des pièces

J'ai fait plusieurs tests pour trouver l'angle nessessaire pour pousser la pièce en dehors du stockage.
En suite, j'ai monté la planche soutenant les pièces sur le reste du distributeur.
Se faisait, j'ai remarqué que la hauteur de certains supports n'etait pas suffisante pour bloquer les pièces supperieure du tube.
Téo a donc fait réimprimer les quelques pièces à changer.

https://user-images.githubusercontent.com/24252743/222415055-9025bd55-9fcc-45de-89fa-96ab0aa629d1.mp4

https://user-images.githubusercontent.com/24252743/222396370-0d3673e9-cfa8-4cbd-a336-67f6afa55b99.mp4

### Réduction du nombre de cables à brancher sur la carte Arduino

Nous avions trop de cables à brancher sur l'arduino mega, j'ai donc du réduire leur nombre.
Pour celà, j'ai utilisé un module HW-061 pour brancher l'écran, ce qui m'as permis de gagner 6 pins sur la carte, ce qui est suffisant ppour tout brancher.

### Correction des problèmes d'alimentation

Au fur et à mesure que je branchait des composants, l'ampérage nessessaire au fonctionnement des éléments était supérieur aux 500mA que peut délivrer la carte Arduino.
Xavier m'as donc dit qu'il fallait procurer 2 alimentations au système, une pour l'arduino (l'inteligence), une pour les composants (la force).
