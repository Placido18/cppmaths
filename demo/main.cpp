/**
 * @file main.cpp
 * @mainpage Documentation du TP C++
 *
 * @section intro_sec Introduction
 * Bienvenue dans la documentation de notre projet de simulation de systèmes physiques.
 *
 * Ce projet utilise l'algorithme de Störmer-Verlet pour simuler l'évolution temporelle
 * de particules (gravité, potentiel de Lennard-Jones).
 */

#include "Univers.hpp"
#include <iostream>
#include <deque>
#include <cmath>

/**
 * @brief Simulation gravitationnelle du système solaire simplifié.
 */
void simulationSolaire() {
    std::deque<Particule> systeme;
    systeme.push_back(Particule(1, "Soleil",  1.0,     {0.0,   0.0, 0.0}, {0.0,    0.0, 0.0}));
    systeme.push_back(Particule(2, "Terre",   3.0e-6,  {0.0,   1.0, 0.0}, {-1.0,   0.0, 0.0}));
    systeme.push_back(Particule(3, "Jupiter", 9.55e-4, {0.0,  5.36, 0.0}, {-0.425, 0.0, 0.0}));
    systeme.push_back(Particule(4, "Halley",  1.0e-14, {34.75, 0.0, 0.0}, {0.0, 0.0296, 0.0}));

    Univers univers(3, 4, systeme, 10000.0, Vecteur(100, 100, 100), {});
    univers.setPhysicsParams(1.0, 1.0, /*gravity=*/true, /*lj=*/false);

    std::cout << "Lancement de la simulation gravitationnelle..." << std::endl;
    univers.evoluerVerlet(0.015, 468.5);
    std::cout << "Simulation terminée." << std::endl;
}

/**
 * @brief Simulation de collision entre deux objets via le potentiel de Lennard-Jones.
 *
 * Paramètres (Lab 4, Question 6) :
 *   epsilon=5, sigma=1, rcut=2.5, dt=0.00005, t_end=19.5
 *   Carré rouge  : 40×40 particules, v=(0,-10)
 *   Rectangle bleu : 160×40 particules, v=(0,0)
 */
void simulationCollision() {
    const double epsilon = 5.0;
    const double sigma   = 1.0;
    const double rcut    = 2.5 * sigma;
    const double dt      = 0.00005;
    const double t_end   = 19.5;

    // Distance inter-particulaire à l'équilibre du puits de LJ
    const double d = std::pow(2.0, 1.0 / 6.0) * sigma;  // ≈ 1.1225

    // Dimensions de l'univers
    const double Lx = 250.0;
    const double Ly = 120.0;   // assez grand pour accueillir les deux objets

    // Grille du rectangle (bleu) : 160×40, positionné en bas, centré en x
    const int nx_rect = 160, ny_rect = 40;
    const double rect_x0 = (Lx - (nx_rect - 1) * d) / 2.0;
    const double rect_y0 = 5.0;

    // Grille du carré (rouge) : 40×40, positionné au-dessus du rectangle
    const int nx_sq = 40, ny_sq = 40;
    const double sq_x0 = (Lx - (nx_sq - 1) * d) / 2.0;
    const double sq_y0 = rect_y0 + (ny_rect - 1) * d + 10.0;  // gap de 10

    std::deque<Particule> particules;
    int id = 1;

    // Rectangle bleu (N2 = 6400), vitesse nulle
    for (int ix = 0; ix < nx_rect; ++ix) {
        for (int iy = 0; iy < ny_rect; ++iy) {
            double x = rect_x0 + ix * d;
            double y = rect_y0 + iy * d;
            particules.push_back(Particule(id++, "bleu", 1.0,
                                           {x, y, 0.0}, {0.0, 0.0, 0.0}));
        }
    }

    // Carré rouge (N1 = 1600), v = (0, -10) — tombe vers le rectangle
    for (int ix = 0; ix < nx_sq; ++ix) {
        for (int iy = 0; iy < ny_sq; ++iy) {
            double x = sq_x0 + ix * d;
            double y = sq_y0 + iy * d;
            particules.push_back(Particule(id++, "rouge", 1.0,
                                           {x, y, 0.0}, {0.0, -10.0, 0.0}));
        }
    }

    Univers univers(2, static_cast<int>(particules.size()), particules,
                    rcut, Vecteur(Lx, Ly, rcut), {});
    univers.setPhysicsParams(epsilon, sigma, /*gravity=*/false, /*lj=*/true);

    std::cout << "Lancement de la simulation de collision (" << particules.size()
              << " particules)..." << std::endl;
    // vtk_freq=1000 → ~390 fichiers VTK pour t_end=19.5
    univers.evoluerVerlet(dt, t_end, 1000);
    std::cout << "Simulation terminée. Fichiers VTK générés." << std::endl;
}

int main() {
    // Choix de la simulation à exécuter
    std::cout << "Choisissez la simulation à exécuter:" << std::endl;
    std::cout << "1. Simulation gravitationnelle du système solaire" << std::endl;
    std::cout << "2. Simulation de collision entre deux objets (Lennard-Jones)" << std::endl;
    std::cout << "Entrez votre choix (1 ou 2): ";
    int choix;
    std::cin >> choix;

    if (choix == 1) {
        simulationSolaire();
    } else if (choix == 2) {
        simulationCollision();
    } else {
        std::cerr << "Choix invalide." << std::endl;
    }

    return 0;
}
