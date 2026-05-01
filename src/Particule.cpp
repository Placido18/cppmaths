/**
 * @file Particule.cpp
 * @brief Code source définissant les comportements d'une Particule.
 */

#include "Particule.hpp"

Particule::Particule(int id, std::string type, double masse, Vecteur pos, Vecteur vitesse) {
    this->id = id;
    this->type = type;
    this->masse = masse;
    this->position = pos;
    this->vitesse = vitesse;
    this->force = Vecteur(0.0, 0.0, 0.0);
}

void Particule::afficher() const{

}

// Getters
int Particule::getId() const { return id; }
std::string Particule::getType() const { return type; }
double Particule::getMasse() const { return masse; }
Vecteur Particule::getPosition() const { return position; }
Vecteur Particule::getVitesse() const { return vitesse; }
Vecteur Particule::getForce() const { return force; }

// Setters
void Particule::setPosition(const Vecteur& pos) { position = pos; }
void Particule::setVitesse(const Vecteur& vit) { vitesse = vit; }
void Particule::setForce(const Vecteur& f) { force = f; }

void Particule::ajouterForce(Vecteur& f) {
    force += f;
}

void Particule::updatePosition(double dt) {
    double m = getMasse();
    Vecteur new_pos = {
        position.getX() + dt * (vitesse.getX() + 0.5 / m * force.getX() * dt),
        position.getY() + dt * (vitesse.getY() + 0.5 / m * force.getY() * dt),
        position.getZ() + dt * (vitesse.getZ() + 0.5 / m * force.getZ() * dt)
    };
    setPosition(new_pos);
}

void Particule::updateVitesse(double dt, const Vecteur& ancienne_force) {
    double m = getMasse();
    Vecteur new_vit = {
        vitesse.getX() + dt * (0.5 / m) * (force.getX() + ancienne_force.getX()),
        vitesse.getY() + dt * (0.5 / m) * (force.getY() + ancienne_force.getY()),
        vitesse.getZ() + dt * (0.5 / m) * (force.getZ() + ancienne_force.getZ())
    };
    setVitesse(new_vit);
}