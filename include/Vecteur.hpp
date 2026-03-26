#ifndef VECTEUR_HPP
#define VECTEUR_HPP

class Vecteur {
    private:
        double x;
        double y;
        double z;

    public:
        Vecteur();
        Vecteur(double x, double y, double z);

        // Getters
        double getX() const;
        double getY() const;
        double getZ() const;

        // Setters
        void setX(double x);
        void setY(double y);
        void setZ(double z);

        // Opérations vectorielles
        Vecteur operator+(const Vecteur& v) const;
        Vecteur operator-(const Vecteur& v) const;
        Vecteur operator*(double scalar) const;
        double dot(const Vecteur& v) const; // Produit scalaire
        Vecteur operator+=(const Vecteur& v);
};

#endif 