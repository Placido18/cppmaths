#include "Cellule.hpp"
#include <vector>

Cellule::Cellule(std::vector<Particule*> particules, std::vector<Cellule*> voisines){
    this->particules = particules;
    this->voisines = voisines;
}