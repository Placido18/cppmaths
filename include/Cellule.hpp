#ifndef CELLULE_HPP
#define CELLULE_HPP

#include "Particule.hpp"
#include <vector>


class Cellule{
    private:
        std::vector<Particule*> particules;//liste de pointeurs vers les particules présentes dans la cellule
        //on utilise des pointeurs pour éviter de copier les particules 
        std::vector<Cellule*> voisines; // pointeurs vers les cellules voisines
        //comme les voisins sont fixés, on pourrait utiliser:
        //std::array<Cellule*, dimension*dimension*dimension> voisines; check
        Vecteur centre; // position du centre de la cellule, utile pour calculer les forces d'interaction

    public:
        Cellule(std::vector<Particule*> particules, std::vector<Cellule*> voisines, Vecteur centre);
        void addVoisine(Cellule* cellule);
        void addParticule(Particule* p);

};


#endif