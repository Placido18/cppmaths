#include "Particule.hpp"
#include <iostream>

Particule::Particule(std::string id, double vitesse, double x, double y, double m) {
    this->x = x;
    this->y = y;
    this->id = id;
    this->vitesse = vitesse;
    this->masse = m;
    this->categorie = "simple";
    this->force = "gravitationnel";
}

void Particule::afficher() const {
    std::cout << "Particule : " << id << ", Masse : " << masse << ", pos : (" << x << ", " << y << ")" << std::endl;
}