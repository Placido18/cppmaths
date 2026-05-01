/**
 * @file main.cpp
 * @mainpage Documentation du TP C++
 * 
 * @section intro_sec Introduction
 * Bienvenue dans la documentation de notre projet de simulation de systèmes gravitationnels.
 * 
 * Ce projet utilise l'algorithme de Störmer-Verlet pour simuler et calculer l'évolution temporelle de particules dans un univers donné (comme notre système solaire).
 */

#include "Univers.hpp"
#include <iostream>
#include <deque>

/**
 * @brief Point d'entrée de la simulation gravitationnelle.
 * 
 * Initialise un système solaire simplifié (Soleil, Terre, Jupiter, Halley) 
 * et lance la simulation avec l'algorithme de Störmer-Verlet.
 * @return 0 si la simulation a réussi.
 */
int main() {
    // création du système gravitationnel
    std::deque<Particule> systeme;
    systeme.push_back(Particule(1, "Soleil", 1.0, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}));
    systeme.push_back(Particule(2, "Terre", 3.0e-6, {0.0, 1.0, 0.0}, {-1.0, 0.0, 0.0}));
    systeme.push_back(Particule(3, "Jupiter", 9.55e-4, {0.0, 5.36, 0.0}, {-0.425, 0.0, 0.0}));
    systeme.push_back(Particule(4, "Halley", 1.0e-14, {34.75, 0.0, 0.0}, {0.0, 0.0296, 0.0}));

    // On crée l'Univers TP2 (pas de cut-off, tout le monde interagit)
    Univers univers_tp2(3, 4, systeme, 10000.0, Vecteur(100, 100, 100), {});

    // Paramètres de l'algorithme
    double dt = 0.015;
    double t_end = 468.5;

    std::cout << "Lancement de la simulation TP2..." << std::endl;
    univers_tp2.evoluerVerlet(dt, t_end);
    std::cout << "Simulation terminée ! Données enregistrées dans positions.txt" << std::endl;

    return 0;
}