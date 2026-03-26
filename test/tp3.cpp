#include "Univers.hpp"
#include <cmath>
#include <random>

// q6) tp3, univers qui contient (2^5)^3 particules
// uniformément distribuées sur le cube [0, 1]^3
int main(){
    int nb_particules = std::pow(std::pow(2, 5), 3);
    std::deque<Particule> particules;

    Univers u = Univers(3, nb_particules, particules);
    
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < nb_particules; i++) {
        double x = dist(mt);
        double y = dist(mt);
        double z = dist(mt);
        Vecteur pos(x, y, z);
        Particule p = Particule(i, "particule", 1, pos, Vecteur(0.0, 0.0, 0.0));
        particules.push_back(p);
    }
}