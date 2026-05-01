#include "Cellule.hpp"
#include <algorithm>

Cellule::Cellule(std::vector<Particule*> particules, std::vector<Cellule*> voisines, Vecteur centre) {
    this->particules = particules;
    this->voisines = voisines;
    this->centre = centre;
}

void Cellule::addVoisine(Cellule* cellule) {
    voisines.push_back(cellule);
}

void Cellule::addParticule(Particule* p) {
    particules.push_back(p);
}

void Cellule::removeParticule(Particule* p) {
    particules.erase(std::remove(particules.begin(), particules.end(), p), particules.end());
}

Vecteur Cellule::getCentre() const {
    return centre;
}

const std::vector<Particule*>& Cellule::getParticules() const {
    return particules;
}