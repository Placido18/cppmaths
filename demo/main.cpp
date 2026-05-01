#include "Particule.hpp"
#include <iostream>
#include <deque>
#include <random>
#include <cmath>
#include <fstream> 

void calculerForces(std::deque<Particule>& particules) {
    for (auto& p : particules) {
        p.setForce({0.0, 0.0, 0.0});
    }

    for (size_t i = 0; i < particules.size(); ++i) {
        for (size_t j = 0; j < particules.size(); ++j) {
            
            // une particule n'exerce pas de force sur elle-même
            if (i == j) continue; 

            Vecteur pos_i = particules[i].getPosition();
            Vecteur pos_j = particules[j].getPosition();

            // calcul du vecteur distance entre les particules i et j
            double rx = pos_j.getX() - pos_i.getX();
            double ry = pos_j.getY() - pos_i.getY();
            double rz = pos_j.getZ() - pos_i.getZ();
            // calcul de la distance
            double dist_carre = rx * rx + ry * ry + rz * rz;
            double dist = std::sqrt(dist_carre);

            if (dist == 0.0) continue;

            // formule de la force gravitationnelle
            double m_i = particules[i].getMasse();
            double m_j = particules[j].getMasse();
            double facteur_force = (m_i * m_j) / (dist * dist * dist);

            // creation du vecteur forve ij;
            Vecteur force_ij = {
                facteur_force * rx,
                facteur_force * ry,
                facteur_force * rz
            };

            particules[i].ajouterForce(force_ij);
        }
    }
}

// algo de stormer verlet pour la simulation du système gravitationnel, 
// le fichier de sortie positions.txt contient à chaque ligne le temps 
// suivi des positions (x, y) de tous les astres du système, séparés par des espaces.

int main() {
    // création du système gravitationnel
    std::deque<Particule> systeme;

    systeme.push_back(Particule(1, "Soleil", 1.0, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}));
    systeme.push_back(Particule(2, "Terre", 3.0e-6, {0.0, 1.0, 0.0}, {-1.0, 0.0, 0.0}));
    systeme.push_back(Particule(3, "Jupiter", 9.55e-4, {0.0, 5.36, 0.0}, {-0.425, 0.0, 0.0}));
    systeme.push_back(Particule(4, "Halley", 1.0e-14, {34.75, 0.0, 0.0}, {0.0, 0.0296, 0.0}));

    // Paramètres de l'algorithme
    double dt = 0.015;
    double t_end = 468.5;
    double t = 0.0;

    std::ofstream fichier("positions.txt");
    if (!fichier.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier !" << std::endl;
        return 1;
    }

    calculerForces(systeme);

    while (t < t_end) {
        t += dt;

        // maj des positions
        for (auto& p : systeme) {
            Vecteur pos = p.getPosition();
            Vecteur vit = p.getVitesse();
            Vecteur force = p.getForce();
            double m = p.getMasse();

            Vecteur new_pos = {
                pos.getX() + dt * (vit.getX() + 0.5 / m * force.getX() * dt),
                pos.getY() + dt * (vit.getY() + 0.5 / m * force.getY() * dt),
                pos.getZ() + dt * (vit.getZ() + 0.5 / m * force.getZ() * dt)
            };
            p.setPosition(new_pos);
        }

        // on écrit le temps, puis toutes les positions (x, y, z) de tous les astres
        fichier << t << " ";
        for (const auto& p : systeme) {
            fichier << p.getPosition().getX() << " " << p.getPosition().getY() << " " << p.getPosition().getZ() << " ";
        }
        fichier << "\n";

        // sauvegarde des anciennes forces
        std::deque<Vecteur> f_old;
        for (auto& p : systeme) {
            f_old.push_back(p.getForce());
        }

        // calcul des nouvelles forces
        calculerForces(systeme);

        // maj des vitesses
        for (size_t k = 0; k < systeme.size(); ++k) {
            Vecteur vit = systeme[k].getVitesse();
            Vecteur force = systeme[k].getForce();
            Vecteur ancienne_force = f_old[k]; 
            double m = systeme[k].getMasse();

            Vecteur new_vit = {
                vit.getX() + dt * (0.5 / m) * (force.getX() + ancienne_force.getX()),
                vit.getY() + dt * (0.5 / m) * (force.getY() + ancienne_force.getY()),
                vit.getZ() + dt * (0.5 / m) * (force.getZ() + ancienne_force.getZ())
            };
            systeme[k].setVitesse(new_vit);
        }

    } 

    fichier.close();
    std::cout << "Simulation terminée ! Données enregistrées dans positions.txt" << std::endl;

    return 0;
}


    // début du tp2, à ajouter dans le main pour voir
    /*
    std::vector<Particule> particleList;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::uniform_real_distribution<double> dist2(0.0, 100.0);

    auto start = std::chrono::steady_clock::now();
    
    for (int i=0; i<20000; i++){
        Particule pi = Particule();

        //pi.afficher();
        
        particleList.push_back(pi);
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    */

// Commentaires tp2 : question 3) j'ai gardé la deque après tests
// qui me semble la moins longue (j'ai testé jusqu'à 20000) mais
// pas grande différence avec la list, vector lui est un peu plus long.