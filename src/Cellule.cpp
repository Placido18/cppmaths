#include "Cellule.hpp"
#include <vector>

Cellule::Cellule(std::vector<Particule*> particules, std::vector<Cellule*> voisines, Vecteur centre) {
    this->particules = particules;
    this->voisines = voisines;
    this->centre = centre;
}

void Cellule::addVoisine(Cellule* cellule) {
    voisines.push_back(cellule);
}