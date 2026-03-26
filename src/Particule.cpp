#include "Particule.hpp"

Particule::Particule(int id, std::string type, double masse, Vec2 pos, Vec2 vitesse) {
    this->id = id;
    this->type = type;
    this->masse = masse;
    this->position = pos;
    this->vitesse = vitesse;
    this->force = {0.0, 0.0};
}

void Particule::afficher() const{

}

// Getters
int Particule::getId() const { return id; }
std::string Particule::getType() const { return type; }
double Particule::getMasse() const { return masse; }
Vec2 Particule::getPosition() const { return position; }
Vec2 Particule::getVitesse() const { return vitesse; }
Vec2 Particule::getForce() const { return force; }

// Setters
void Particule::setPosition(const Vec2& pos) { position = pos; }
void Particule::setVitesse(const Vec2& vit) { vitesse = vit; }
void Particule::setForce(const Vec2& f) { force = f; }

void Particule::ajouterForce(Vec2& f) {
    force[0] += f[0];
    force[1] += f[1];
    force[2] += f[2];
}