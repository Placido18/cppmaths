# Simulation N-Corps et Potentiel de Lennard-Jones

**Auteurs :** Neuilly Placide, Meniane Romain
**Cours :** C++ pour les mathématiques appliquées – Grenoble INP (Ensimag 2A)

## Description du projet
Ce projet implémente un moteur de simulation physique de particules en C++ (1D, 2D et 3D). Il permet de simuler des interactions gravitationnelles ainsi que des dynamiques d'interactions intermoléculaires modélisées par le **potentiel de Lennard-Jones**.

**Fonctionnalités et implémentations physiques :**
* **Intégrateur Störmer-Verlet** : Un algorithme numérique symplectique d'ordre 2 pour calculer la position et la vitesse avec une bonne conservation de l'énergie à long terme.
* **Découpage spatial (Maillage en cellules)** : Afin d'éviter un calcul des forces en $O(N^2)$, l'espace est partitionné en une grille de cellules de taille $r_{cut}$ (rayon de coupure). La complexité est ainsi réduite à $O(N)$ en ne calculant que les interactions avec les particules des cellules voisines.
* **Simulation de collision** : Implémentation d'un scénario de collision élastique entre deux groupes de particules (par ex. un carré percutant un rectangle).
* **Modulable** : Bascule facile entre la gravité macroscopique et le potentiel LJ. Différentes conditions aux limites supportées (réflexion, absorption, périodique, répulsion).

## Compilation

Le projet utilise **CMake** pour la configuration et la compilation. Depuis la racine du projet :

mkdir build
cd build
cmake ..
make

## Exécution et Tests

### Programme principal (Simulation)
Une fois compilé, vous pouvez lancer l'exécutable principal généré dans le dossier 'build', tapez : demo/main.

### Tests de performances (TP3)
Pour compiler et exécuter spécifiquement les tests de performance liés à l'insertion mémoire (dans une structure `std::deque`) et la mesure temporelle du calcul des interactions :
test/tp3


### Tests unitaires
L'infrastructure de tests s'appuie sur **Google Test (GTest)**. Beaucoup de tests vérifient toutes nos classes, l'initialisation de maillages ... 
Pour lancer l'ensemble des tests : ctest
Ou bien : test/tests



## Visualisation

La simulation produit des données pouvant être exploitées visuellement :

1. **ParaView (Export VTK)** :  
   La méthode `Univers::sauvegarderVTK()` exporte régulièrement l'état de l'univers sous forme de fichiers XML non structurés (`.vtu`).  
   * Chargez ces fichiers dans ParaView.
   * Utilisez l'attribut `Groupe` (ex: 0=bleu, 1=rouge) pour colorer et identifier les entités entrant en collision.


## Structure du répertoire
* `include/` : Fichiers d'en-tête (Univers, Cellule, Particule, Vecteur).
* `src/` : Implémentations et logique métier.
* `test/` : Benchmarks de performances, infrastructure GTest et scripts Python.
* `doc/` : pour générer la doc et les différents rapports, faire : make doc. Ensuite ouvrir index.html dans docu/index/ pour voir toute l'architecture du projet. 