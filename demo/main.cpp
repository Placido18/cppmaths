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
#include <stdexcept>

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

/**
 * @brief Simulation de collision entre une balle et un sol (Lab 6, Question 6).
 *
 * Paramètres :
 *   L1=250, L2=180, ε=1, σ=1, m=1, N1=395 (balle), N2=17227 (sol),
 *   rcut=2.5σ, δt=0.0005, G=-12, E_c^D=0.005*(N1+N2), t_end=29.5
 *
 * La balle (N1 particules disposées en disque) tombe par gravité sur un sol
 * rectangulaire (N2 particules). Les vitesses sont rescalées toutes les 1000
 * itérations pour maintenir l'énergie cinétique proche de E_c^D.
 */
void simulationCollisionObjets() {
    const double epsilon  = 1.0;
    const double sigma    = 1.0;
    const double rcut     = 2.5 * sigma;
    const double dt       = 0.0005;
    const double t_end    = 29.5;
    const double G        = -12.0;
    const int    N1       = 395;
    const int    N2       = 17227;
    const double Ec_cible = 0.005 * (N1 + N2);

    // Distance inter-particulaire à l'équilibre du puits de LJ
    const double d = std::pow(2.0, 1.0/6.0) * sigma;  // ≈ 1.1225

    const double Lx = 250.0;
    const double Ly = 180.0;

    std::deque<Particule> particules;
    int id = 1;

    // --- Sol rectangulaire (N2 particules, type "bleu") ---
    // Positionné en bas du domaine, centré en x
    const int nx_sol = static_cast<int>(Lx / d);         // ≈ 222 colonnes
    const double sol_x0 = (Lx - (nx_sol - 1) * d) / 2.0;
    const double sol_y0 = d;                              // une marge en bas

    int placed_sol = 0;
    for (int iy = 0; placed_sol < N2; ++iy) {
        for (int ix = 0; ix < nx_sol && placed_sol < N2; ++ix) {
            double x = sol_x0 + ix * d;
            double y = sol_y0 + iy * d;
            particules.push_back(Particule(id++, "bleu", 1.0,
                                           {x, y, 0.0}, {0.0, 0.0, 0.0}));
            ++placed_sol;
        }
    }

    // Hauteur du sommet du sol
    const int ny_sol = static_cast<int>(std::ceil(static_cast<double>(N2) / nx_sol));
    const double top_sol = sol_y0 + (ny_sol - 1) * d;

    // --- Balle (N1 particules disposées en disque, type "rouge") ---
    // Rayon physique estimé depuis N1 et la densité du réseau carré
    const double r_balle = std::sqrt(static_cast<double>(N1) / M_PI) * d;
    const double cx = Lx / 2.0;
    const double cy = top_sol + r_balle + 3.0 * d;       // gap de 3d au-dessus du sol

    int placed_balle = 0;
    const int n_grid = static_cast<int>(r_balle / d) + 2;
    for (int iy = -n_grid; iy <= n_grid && placed_balle < N1; ++iy) {
        for (int ix = -n_grid; ix <= n_grid && placed_balle < N1; ++ix) {
            double rx = ix * d;
            double ry = iy * d;
            if (rx*rx + ry*ry <= r_balle*r_balle) {
                particules.push_back(Particule(id++, "rouge", 1.0,
                                               {cx + rx, cy + ry, 0.0}, {0.0, -10.0, 0.0}));
                ++placed_balle;
            }
        }
    }

    std::cout << "Balle : " << placed_balle << " particules (cible " << N1 << ")" << std::endl;
    std::cout << "Sol   : " << placed_sol   << " particules (cible " << N2 << ")" << std::endl;

    Univers univers(2, static_cast<int>(particules.size()), particules,
                    rcut, Vecteur(Lx, Ly, rcut), {});
    univers.setPhysicsParams(epsilon, sigma, /*gravity=*/false, /*lj=*/true);
    univers.setConditionsLimites(ConditionLimite::REFLEXION,
                                 ConditionLimite::REFLEXION,
                                 ConditionLimite::REFLEXION);
    univers.setChampGravite(G);
    univers.setEnergieCinetiqueCible(Ec_cible);

    std::cout << "Lancement de la simulation balle/sol ("
              << particules.size() << " particules, t_end=" << t_end << ")..." << std::endl;
    // vtk_freq=1000 → ~59 fichiers VTK pour t_end=29.5 et dt=0.0005
    univers.evoluerVerlet(dt, t_end, 1000);
    std::cout << "Simulation terminée. Fichiers VTK générés." << std::endl;
}

/**
 * @brief Démo des conditions aux limites (Q1-Q2).
 *
 * Lance une petite simulation 2D avec quelques particules pour tester
 * chaque type de condition aux limites.
 */
void demoConditionsLimites() {
    const double sigma   = 1.0;
    const double rcut    = 2.5 * sigma;
    const double dt      = 0.001;
    const double t_end   = 5.0;

    // Choix du type de condition
    std::cout << "Choisissez le type de condition aux limites :" << std::endl;
    std::cout << "  1. Réflexion" << std::endl;
    std::cout << "  2. Absorption" << std::endl;
    std::cout << "  3. Périodique" << std::endl;
    std::cout << "  4. Réflexion par potentiel" << std::endl;
    std::cout << "Votre choix : ";
    int choix_cl;
    std::cin >> choix_cl;

    ConditionLimite cl;
    switch (choix_cl) {
        case 1: cl = ConditionLimite::REFLEXION;           break;
        case 2: cl = ConditionLimite::ABSORPTION;          break;
        case 3: cl = ConditionLimite::PERIODIQUE;          break;
        case 4: cl = ConditionLimite::REFLEXION_POTENTIEL; break;
        default:
            std::cerr << "Choix invalide." << std::endl;
            return;
    }

    // Petite grille de 5×5 particules avec vitesses aléatoires vers les bords
    std::deque<Particule> particules;
    const double Lx = 20.0, Ly = 20.0;
    const double d = std::pow(2.0, 1.0/6.0) * sigma;
    int pid = 1;
    for (int ix = 0; ix < 5; ++ix) {
        for (int iy = 0; iy < 5; ++iy) {
            double x = 2.0 + ix * d * 2.0;
            double y = 2.0 + iy * d * 2.0;
            // Vitesse dirigée vers l'extérieur
            double vx = (ix - 2) * 3.0;
            double vy = (iy - 2) * 3.0;
            particules.push_back(Particule(pid++, "bleu", 1.0,
                                           {x, y, 0.0}, {vx, vy, 0.0}));
        }
    }

    Univers univers(2, static_cast<int>(particules.size()), particules,
                    rcut, Vecteur(Lx, Ly, rcut), {});
    univers.setPhysicsParams(1.0, sigma, false, true);
    univers.setConditionsLimites(cl, cl, cl);

    std::cout << "Simulation avec condition aux limites (25 particules, t=" << t_end << ")..." << std::endl;
    univers.evoluerVerlet(dt, t_end, 100);
    std::cout << "Simulation terminée. Fichiers VTK générés." << std::endl;
}

int main() {
    std::cout << "Choisissez la simulation à exécuter:" << std::endl;
    std::cout << "1. Simulation gravitationnelle du système solaire" << std::endl;
    std::cout << "2. Simulation de collision entre deux objets (Lennard-Jones, TP4)" << std::endl;
    std::cout << "3. Simulation balle/sol avec gravité (Lab 6, Q6)" << std::endl;
    std::cout << "4. Démo conditions aux limites (Lab 6, Q1-2)" << std::endl;
    std::cout << "Entrez votre choix (1-4): ";
    int choix;
    std::cin >> choix;

    try {
        if (choix == 1) {
            simulationSolaire();
        } else if (choix == 2) {
            simulationCollision();
        } else if (choix == 3) {
            simulationCollisionObjets();
        } else if (choix == 4) {
            demoConditionsLimites();
        } else {
            std::cerr << "Choix invalide." << std::endl;
            return 1;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Erreur de paramètre : " << e.what() << std::endl;
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << "Erreur d'exécution : " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Erreur inattendue : " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
