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