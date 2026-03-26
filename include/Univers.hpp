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
};

#endif