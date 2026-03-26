#include <iostream>
#include <deque>
#include <random>
#include <cmath>
#include <chrono>
#include <vector>
#include "Univers.hpp"
#include "Particule.hpp"

/**
 * Fonction utilitaire pour générer un Univers aléatoire
 * Les particules sont uniformément distribuées sur le cube [0, 1]^3 (Question 6) [cite: 274]
 */
Univers genererUniversAleatoire(int k) {
    int N_side = std::pow(2, k);
    int total_particles = std::pow(N_side, 3);
    
    std::deque<Particule> particules;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < total_particles; ++i) {
        Vecteur pos(dist(mt), dist(mt), dist(mt));
        Vecteur vit(0.0, 0.0, 0.0);
        particules.push_back(Particule(i, "particule", 1.0, pos, vit));
    }

    return Univers(3, total_particles, particules);
}

int main() {
    std::cout << "test performance" << std::endl;

    // q7
    // Test pour (2^5)^3
    int N_Q7 = std::pow(32, 3);
    auto start_ins = std::chrono::steady_clock::now();
    
    std::deque<Particule> test_ins;
    for (int i = 0; i < N_Q7; ++i) {
        test_ins.push_back(Particule(i, "test", 1.0, {0,0,0}, {0,0,0}));
    }
    
    auto end_ins = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_ins = end_ins - start_ins;
    std::cout << "[Q7] Insertion de " << N_Q7 << " particules (deque) : " 
              << time_ins.count() << " s" << std::endl;

    std::cout << "\n--- QUESTION 8 : CALCUL DES INTERACTIONS ---" << std::endl;
    std::cout << "Nb Particules (N) | Temps Calcul Forces (s)" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    //q8, k=3 et k=7
    // Pour k=7. Le calcul sera extrêmement long.
    // On limite l'affichage si le temps devient excessif.
    for (int k = 3; k <= 7; ++k) {
        int N = std::pow(std::pow(2, k), 3);
        
        Univers u = genererUniversAleatoire(k);
        
        // Mesure du temps pour calculer les forces
        auto start_f = std::chrono::steady_clock::now();
        u.calculerForces();
        auto end_f = std::chrono::steady_clock::now();
        
        std::chrono::duration<double> time_f = end_f - start_f;
        std::cout << N << " (" << k << ") \t\t| " << time_f.count() << " s" << std::endl;

        // Condition de sécurité : on arrête si le calcul dépasse 30 secondes pour le TP
        if (time_f.count() > 30.0) {
            std::cout << "=> Test interrompu : N trop grand pour un temps raisonnable" << std::endl;
            break;
        }
    }

    return 0;
}