/**
 * @file LennardJonesPotential.cpp
 * @brief Implémentation du potentiel de Lennard-Jones.
 */

#include <iostream>
#include <cmath>
#include <fstream>

/**
 * @brief Calcule le potentiel de Lennard-Jones entre deux particules.
 * 
 * @param r Distance entre les particules.
 * @param epsilon Profondeur du puits de potentiel.
 * @param sigma Distance à laquelle le potentiel interparticulaire est nul.
 * @param rcut Rayon de coupure pour limiter la portée des interactions.
 * @return La valeur calculée du potentiel (0 si r > rcut).
 */
double calculLennardJones(double r, double epsilon, double sigma, double rcut) {
    if (r > rcut) {
        return 0; // Potentiel nul au-delà de la distance de coupure    
    }
    //on prefere r*r pour les puissances moins couteux que la fonction pow
    if (r==0) {
        return 0;
    }
    double r2 = r * r;
    double sigma2 = sigma * sigma;
    double sigma6 = sigma2 * sigma2 * sigma2; // sigma^6
    double sigma12 = sigma6 * sigma6; // sigma^12
    double r6 = r2 * r2 * r2; 
    double r12 = r6 * r6; 
    return 4 * epsilon * (sigma12 / r12 - sigma6 / r6);
}

// tracé du potentiel de Lennard-Jones en fonction de la distance r:Z
/*
int main() {
    double epsilon = 1;
    double sigma = 1;

    double r_min = 0;
    double r_max = 5;

    double pas_r = 0.1;

    std::ofstream fichier("lennard_jones_potential.txt");
    if (!fichier.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier !" << std::endl;
        return 1;
    }

    for (double r = r_min; r <= r_max; r += pas_r) {
        double potential = calculLennardJones(r, epsilon, sigma );
        fichier << r << " " << potential << "\n";
    }

    fichier.close();

    return 0;
}
    
*/