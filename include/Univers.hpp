#ifndef UNIVERS_HPP
#define UNIVERS_HPP
#include <deque>
#include "Particule.hpp"

class Univers {
    private:
        int dimension;
        int nb_particules;
        std::deque<Particule> particules;


    public:
        Univers(int dimension, int nb_particules, std::deque<Particule> particules);

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

};

#endif