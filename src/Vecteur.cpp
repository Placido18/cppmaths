/// créer une classe vecteur de dimension 3
#include "Vecteur.hpp"
Vecteur::Vecteur(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}


// Getters
double Vecteur::getX() const { return x; }
double Vecteur::getY() const { return y; }
double Vecteur::getZ() const { return z; }

// Setters
void Vecteur::setX(double x) { this->x = x; }
void Vecteur::setY(double y) { this->y = y; }
void Vecteur::setZ(double z) { this->z = z; }

// Opérations vectorielles
Vecteur Vecteur::operator+(const Vecteur& v) const {
    return Vecteur(x + v.x, y + v.y, z + v.z);
}

Vecteur Vecteur::operator-(const Vecteur& v) const {
    return Vecteur(x - v.x, y - v.y, z - v.z);
}

Vecteur Vecteur::operator*(double scalar) const {
    return Vecteur(x * scalar, y * scalar, z * scalar);
}

// Produit scalaire d'un vecteur avec un autre
double Vecteur::dot(const Vecteur& v) const {
    return x * v.x + y * v.y + z * v.z;
}
