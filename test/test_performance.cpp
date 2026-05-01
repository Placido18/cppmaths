/**
 * @file test_performance.cpp
 * @brief Programme de test des performances d'insertion.
 * 
 * Évalue les performances d'utilisation des conteneurs std::deque 
 * pour le stockage d'un grand nombre de particules.
 */
#include "Particule.hpp"
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <random>
#include <chrono>

/**
 * @brief Point d'entrée du test de performance.
 * 
 * Insère 20 000 particules aléatoires dans une structure std::deque 
 * et chronomètre le temps d'exécution.
 * @return 0 si le test s'est bien déroulé.
 */
int main() {
    std::deque<Particule> particleList;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    auto start = std::chrono::steady_clock::now();
    
    for (int i=0; i<20000; i++){
        Particule pi(i, "Test", 1.0, {dist(mt), dist(mt), dist(mt)}, {0.0, 0.0, 0.0});
        particleList.push_back(pi);
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Temps d'insertion (deque, 20000 elements) : " << elapsed_seconds.count() << "s\n";

    return 0;
}