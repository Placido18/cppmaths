#include "Univers.hpp"
#include "Particule.hpp"
#include "Vecteur.hpp"
#include <iostream>
#include <deque>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

// Fonction utilitaire pour exporter au format .xyz (lisible par Ovito ou VMD)
void sauvegarderXYZ(const Univers& univers, int etape, double temps) {
    std::ostringstream filename;
    filename << "frame_" << std::setw(5) << std::setfill('0') << etape << ".xyz";
    std::ofstream fichier(filename.str());

    if (fichier.is_open()) {
        // Ligne 1: Nombre de particules
        fichier << univers.getParticules().size() << "\n";
        // Ligne 2: Commentaire (utile pour afficher le temps)
        fichier << "Temps: " << temps << "\n";
        
        // Lignes suivantes: Type X Y Z
        for (const auto& p : univers.getParticules()) {
            fichier << p.getType() << " " 
                    << p.getPosition().getX() << " " 
                    << p.getPosition().getY() << " " 
                    << p.getPosition().getZ() << "\n";
        }
    }
}

int main() {
    // ---------------------------------------------------------
    // Paramètres de la simulation (Lab 4 - Section 3)
    // ---------------------------------------------------------
    double epsilon = 5.0;            //  (Pensez à le modifier dans Univers::calculerForces !)
    double sigma = 1.0;              // [cite: 163]
    double m = 1.0;                  // [cite: 164]
    double rcut = 2.5 * sigma;       // [cite: 167]
    double dt = 0.00005;             // [cite: 168]
    double t_end = 19.5;             // [cite: 174]
    
    // Domaine: L1=250, L2=40. On va utiliser une boîte 2D de 250x250 pour avoir de la place en hauteur.
    Vecteur Ld(250.0, 250.0, 0.0);   
    int dimension = 2;

    // Distance d'équilibre entre les particules
    double d_eq = std::pow(2.0, 1.0/6.0) / sigma; // [cite: 172]

    std::deque<Particule> particules;
    int id_counter = 0;

    // ---------------------------------------------------------
    // Création du Rectangle Bleu (Cible)
    // ---------------------------------------------------------
    // 160 x 40 particules [cite: 171]
    int nx_rect = 160;
    int ny_rect = 40;
    
    // On le centre sur l'axe X (largeur totale de 250)
    double start_x_rect = (250.0 - (nx_rect * d_eq)) / 2.0; 
    double start_y_rect = 20.0; // On le place en bas du domaine

    for (int i = 0; i < nx_rect; ++i) {
        for (int j = 0; j < ny_rect; ++j) {
            double px = start_x_rect + i * d_eq;
            double py = start_y_rect + j * d_eq;
            
            Vecteur pos(px, py, 0.0);
            Vecteur vitesse(0.0, 0.0, 0.0); // Au repos
            
            particules.emplace_back(id_counter++, "Bleu", m, pos, vitesse);
        }
    }

    // ---------------------------------------------------------
    // Création du Carré Rouge (Projectile)
    // ---------------------------------------------------------
    // 40 x 40 particules [cite: 171]
    int nx_carre = 40;
    int ny_carre = 40;
    
    // On le centre au-dessus du rectangle
    double start_x_carre = (250.0 - (nx_carre * d_eq)) / 2.0; 
    double start_y_carre = start_y_rect + (ny_rect * d_eq) + 20.0; // 20 unités au-dessus du rectangle

    for (int i = 0; i < nx_carre; ++i) {
        for (int j = 0; j < ny_carre; ++j) {
            double px = start_x_carre + i * d_eq;
            double py = start_y_carre + j * d_eq;
            
            Vecteur pos(px, py, 0.0);
            Vecteur vitesse(0.0, -10.0, 0.0); // v = (0, -10) vers le bas pour l'impact [cite: 165, 170]
            
            particules.emplace_back(id_counter++, "Rouge", m, pos, vitesse);
        }
    }

    // ---------------------------------------------------------
    // Initialisation de l'Univers
    // ---------------------------------------------------------
    std::vector<Cellule> cellules_vides;
    Univers univers(dimension, particules.size(), particules, rcut, Ld, cellules_vides);
    
    std::cout << "Initialisation du maillage spatial..." << std::endl;
    univers.initialiserMaillage();
    univers.assignerParticulesAuxCellules();

    std::ofstream fichier_sortie("simulation.txt");
if (!fichier_sortie.is_open()) {
    std::cerr << "Erreur lors de la création du fichier simulation.txt" << std::endl;
    return 1;
}

double t = 0.0;
int etape = 0;
int output_frequency = 2000; // Fréquence de sauvegarde

// Sauvegarde initiale
univers.calculerForces();
sauvegarderTXT(fichier_sortie, univers, t);

while (t < t_end) {
    univers.avancer(dt);
    
    std::vector<Vecteur> f_old;
    f_old.reserve(univers.getParticules().size());
    for (const auto& p : univers.getParticules()) {
        f_old.push_back(p.getForce());
    }
    
    univers.calculerForces();
    
    // updateVitesse nécessite probablement d'utiliser les indices ou des itérateurs
    // Adapte cette partie selon ta structure exacte
    for (size_t i = 0; i < univers.getParticules().size(); ++i) {
        // univers.getParticules()[i].updateVitesse(dt, f_old[i]); 
        // Note: si getParticules() renvoie une copie ou un const, il te faut une méthode pour modifier la particule.
    }

    t += dt;
    etape++;

    if (etape % output_frequency == 0) {
        std::cout << "Temps : " << t << " / " << t_end << "\r" << std::flush;
        sauvegarderTXT(fichier_sortie, univers, t);
    }
}

fichier_sortie.close();
std::cout << "\nSimulation terminée. Données écrites dans simulation.txt" << std::endl;
return 0;
}

// Fonction pour exporter dans un format texte simple : Temps ID Type X Y
void sauvegarderTXT(std::ofstream& fichier, const Univers& univers, double temps) {
    if (fichier.is_open()) {
        for (const auto& p : univers.getParticules()) { // Assure-toi d'avoir un getter ou un accès public
            fichier << temps << " " 
                    << p.getId() << " " 
                    << p.getType() << " " 
                    << p.getPosition().getX() << " " 
                    << p.getPosition().getY() << "\n";
        }
    }
}