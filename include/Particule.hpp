#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include <string>
#include <array>
#include "Vecteur.hpp"

using Vec2 = std::array<double, 2>;

class Particule{
    private:
        int id;
        std::string type;
        double masse;
        Vecteur position;
        Vecteur vitesse;
        Vecteur force;

    public:
        void afficher() const;
        Particule(int id, std::string type, double masse, Vecteur pos, Vecteur vitesse);

        // Getters
        int getId() const;
        std::string getType() const;
        double getMasse() const;
        Vecteur getPosition() const;
        Vecteur getVitesse() const;
        Vecteur getForce() const; 
        
        // Setters
        void setPosition(const Vecteur& pos);
        void setVitesse(const Vecteur& vitesse);
        void setForce(const Vecteur& f);

        void ajouterForce(Vecteur& f);
};

#endif