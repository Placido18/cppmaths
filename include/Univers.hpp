#ifndef UNIVERS_HPP
#define UNIVERS_HPP
#include <deque>
#include "Particule.hpp"
#include "Vecteur.hpp"
#include "Cellule.hpp"

#include <vector>

class Univers {
    private:
        int dimension;
        int nb_particules;

        //les particules sont stockées dans l'univers,
        //les cellules contiennent des pointeurs vers les particules, mais ne les possèdent pas
        //voir s'il n'y a pas de problème de cohérence entre deque et vector, check
        std::deque<Particule> particules;

        //ajout du rayon de coupe et d'une longueur caractéristique

        double rcut;// c'est à la fois la distance de coupure pour les interactions et donc la taille des cellules du maillage
        Vecteur Ld; 

        //un univers possède une liste de cellules
        std::vector<Cellule> cellules;
        //std::array<Cellule, dimension*dimension*dimension> cellules; check
        
        



    public:
        Univers(int dimension, int nb_particules, std::deque<Particule> particules, double rcut, Vecteur Ld, std::vector<Cellule> cellules);

        // pour faire avancer les particules pendant un temps de dt
        void avancer(double dt);

        // permet d'afficher les positions des particules à un instant t
        void afficher(double t) const;

        // calcul les forces d'interaction
        void calculerForces() ;

        // méthode d'évolution
        void evoluer(double dt, double t_end);

        // change la vitesse de toutes les particules (supposées identiques)
        void appliquerVitesse(double vitesse);
        

        // méthode pour accéder à la dimension de l'univers
        int getDimension() const;

        void initialiserMaillage() ; // méthode pour initialiser le maillage de cellules dans l'univers, à appeler au début de la simulation

};

#endif