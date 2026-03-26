#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include <string>

class Particule{
    private:
        double x, y;
        double vitesse;
        double masse;
        std::string id;
        std::string categorie;
        std::string force;

    public:
        void afficher() const;
        Particule(std::string id, double vitesse, double x, double y, double m);
};

#endif