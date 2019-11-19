# MARBLE WARS

## Auteur
DESNOUST Nicolas
nicolas.desnoust@etu.univ-amu.fr

## Pour lancer le jeu
Exécuter la commande "make all" pour compiler le programme et "./marbleWars" pour l'exécuter.
Un écran d'accueil apparait, dès l'appui d'une touche le jeu se lance.

![Screenshot de l'écran d'accueil](screenshots/capt01.png?raw=true "Ecran d'accueil")

## Fonctionnalités
Il est possible d'éditer les niveaux, de quitter le jeu ("q"), de le mettre en pause ("p").
Le jeu est jouable à l'aide de la souris (clic gauche pour lancer des billes) et la barre espace pour changer les billes dans le canon. A chaque niveau terminé le joueur passe directement au suivant, si tous les niveaux ont été terminés c'est le premier qui sera de nouveau chargé avec une difficulté supérieure et ce jusqu'a ce que le joueur perde toutes ses vies (affichées à l'écran).

![Screenshot du gameplay 1](screenshots/capt02.png?raw=true "Gameplay 1")

Lorsque le joueur perd on affiche le tableau des scores et on lui demande son nom, puis le jeu retourne à l'écran d'accueil. La fenêtre d'affichage est de dimensions fixes et centrée au millieu de l'écran pour respecter les dimensions des fonds d'écrans. Les combos n'ont pas étés implémentés, de ce fait après une explosion de 3 billes ou plus si d'autres billes doivent exploser elles le feront juste après, sans que le train de billes ne recule. Une animation d'explosion des billes a été rajoutée. 

![Screenshot du gameplay 2](screenshots/capt03.png?raw=true "Gameplay 2")

Pour éditer un niveau il faut charger le niveau que l'on souhaite modifier, faire les modifications nécessaires et le sauvegarder sinon les modifications ne seront pas prises en compte. 

![Screenshot du mode d'édition](screenshots/capt04.png?raw=true "Mode d'édition")
