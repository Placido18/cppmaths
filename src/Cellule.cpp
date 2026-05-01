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

std::vector<Cellule*> Cellule::getVoisines() const {
    return voisines;
}

std::vector<Particule*> Cellule::getParticules() const {
    return particules;
}

Vecteur Cellule::getCentre() const {
    return centre;
}

void Cellule::removeParticule(Particule* p) {
    // On cherche la particule dans le vecteur
    auto it = std::find(particules.begin(), particules.end(), p);
    if (it != particules.end()) {
        // Si on la trouve, on la retire
        particules.erase(it);
    }
}