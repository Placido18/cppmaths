#include "Univers.hpp"
#include <cmath>
#include <random>
#include <chrono>
#include <iostream>

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


    /// question 6) et 7) de test de l'univers

    const int N_side = 32; // (2^5)
    const int total_particles = N_side * N_side * N_side; // 32 768

    auto start = std::chrono::steady_clock::now();

    std::deque<Particule> test_container;
    for (int i = 0; i < total_particles; ++i) {
        // Insertion d'une particule type
        test_container.push_back(Particule(i, "Test", 1.0, {0,0,0}, {0,0,0}));
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Temps pour insérer " << total_particles << " particules dans deque: " << elapsed.count() << "s" << std::endl;
}