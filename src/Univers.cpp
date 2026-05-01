/**
 * @file Univers.cpp
 * @brief Implémentation de la classe Univers et de la simulation physique.
 */

#include "Univers.hpp"
#include <iostream>
#include <cmath>
#include <fstream>


/// on utilise deque pour la collection de particules car elle possède les meilleures performances (voir comparaison avec list et vector dand le main)
Univers::Univers(int dimension, int nb_particules, std::deque<Particule> particules, double rcut, Vecteur Ld, std::vector<Cellule> cellules) {
    this->dimension = dimension;
    this->nb_particules = nb_particules;
    this->particules = particules;
    this->rcut = rcut;
    this->Ld = Ld;
    this->cellules = cellules;

    

}

// pas encore mis l'impact des forces
void Univers::avancer(double dt) {
    for (auto& p : particules) {
        Vecteur pos = p.getPosition();
        Vecteur vit = p.getVitesse();

        double new_x = pos.getX() + vit.getX() * dt;
        double new_y = pos.getY();
        double new_z = pos.getZ();

        if (dimension >= 2) {
            new_y += vit.getY() * dt;
        }

        if (dimension >= 3) {
            new_z += vit.getZ() * dt;
        }

        Vecteur new_pos(new_x, new_y, new_z);
        p.setPosition(new_pos);
    }
}

void Univers::afficher(double t) const {
    std::cout << "Temps: " << t << "s\n";
    for (const auto& p : particules) {
        std::cout << "Particule " << p.getId() << " (" << p.getType() << "): Position = (" 
                  << p.getPosition().getX();

        if (dimension >= 2) {
            std::cout << ", " << p.getPosition().getY();
        }
        if (dimension == 3) {
            std::cout << ", " << p.getPosition().getZ();
        }
        
        std::cout << ")\n";
    }
}

void Univers::evoluer(double dt, double t_end) {
    double t = 0.0;
    while (t < t_end) {
        calculerForces(); // dépend du système
        avancer(dt);
        t += dt;
    }
}

void Univers::evoluerVerlet(double dt, double t_end) {
    double t = 0.0;
    std::ofstream fichier("positions.txt");
    calculerForces();

    while (t < t_end) {
        t += dt;
        
        // 1. Mise à jour des positions
        for (auto& p : particules) { p.updatePosition(dt); }
        
        // Sauvegarde dans le fichier
        if (fichier.is_open()) {
            fichier << t << " ";
            for (const auto& p : particules) {
                fichier << p.getPosition().getX() << " " << p.getPosition().getY() << " " << p.getPosition().getZ() << " ";
            }
            fichier << "\n";
        }

        // 2. Sauvegarde des anciennes forces et calcul des nouvelles
        std::deque<Vecteur> f_old;
        for (auto& p : particules) { f_old.push_back(p.getForce()); }
        calculerForces();
        
        // 3. Mise à jour des vitesses
        for (size_t i = 0; i < particules.size(); ++i) {
            particules[i].updateVitesse(dt, f_old[i]);
        }
    }
}

// les particules ont la même vitesse
void Univers::appliquerVitesse(double vitesse) {
    for (auto& p : particules) {
        Vecteur vit = p.getVitesse();
        double norm = std::sqrt(vit.getX() * vit.getX() + vit.getY() * vit.getY() + vit.getZ() * vit.getZ());
        if (norm > 0) {
            double scale = vitesse / norm;
            Vecteur new_vit(vit.getX() * scale, vit.getY() * scale, vit.getZ() * scale);
            p.setVitesse(new_vit);
        }
    }
}

void Univers::calculerForces() {
    // Remise à zéro des forces avant chaque calcul
    for (auto& p : particules) {
        p.setForce({0.0, 0.0, 0.0});
    }
    
    for (size_t i = 0; i < particules.size() - 1; ++i) {
        //position de la particule courante
        Vecteur pos_i = particules[i].getPosition();

        //on doit parcourir toutes les cellules du voisinage de la cellule de la particule i
        //cad les cellules dont le centre est à une distance inférieure à rcut de la cellule de la particule i


        
        //On ne fait ca que pour les particules qui appartiennent aux cellules qui se situe dans le voisinage de la cellule de la particule
        for (size_t j = i + 1; j < particules.size(); ++j) {
            
            Vecteur pos_j = particules[j].getPosition();
            Vecteur r_ij = pos_j - pos_i;
            double distance = std::sqrt(r_ij.getX() * r_ij.getX() + r_ij.getY() * r_ij.getY() + r_ij.getZ() * r_ij.getZ());
            double m_i = particules[i].getMasse();
            double m_j = particules[j].getMasse();
            if (distance > 0) {
                double facteur_force = (m_i * m_j) / (distance * distance * distance);
                // creation du vecteur forve ij;
                Vecteur force_ij = {
                    facteur_force * r_ij.getX(),
                    facteur_force * r_ij.getY(),
                    facteur_force * r_ij.getZ()
                };
                particules[i].ajouterForce(force_ij);
                Vecteur force_ji = force_ij * -1;
                particules[j].ajouterForce(force_ji);
            }
        }
    }

}

int Univers::getDimension() const {
    return dimension;

}

void Univers::initialiserMaillage() {
    // Calcul de base pour X (commun à toutes les dimensions)
    int ncd_x = Ld.getX() / rcut;
    if (ncd_x == 0) ncd_x = 1; // Sécurité si le domaine est plus petit que rcut

    if (dimension == 1) {
        cellules.reserve(ncd_x);
        for (int i = 0; i < ncd_x; ++i) {
            Vecteur centre((i + 0.5) * rcut, 0, 0);
            cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(), centre);
        }

        for (int i = 0; i < ncd_x; ++i) {
            for (int di = -1; di <= 1; ++di) {
                if (di == 0) continue; // On s'ignore soi-même
                
                int ni = i + di;
                if (ni >= 0 && ni < ncd_x) {
                    cellules[i].addVoisine(&cellules[ni]);
                }
            }
        }
    } 
    else if (dimension == 2) {
        int ncd_y = Ld.getY() / rcut;
        if (ncd_y == 0) ncd_y = 1;

        cellules.reserve(ncd_x * ncd_y);
        for (int i = 0; i < ncd_x; ++i) {
            for (int j = 0; j < ncd_y; ++j) {
                Vecteur centre((i + 0.5) * rcut, (j + 0.5) * rcut, 0);
                cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(), centre);
            }
        }

        for (int i = 0; i < ncd_x; ++i) {
            for (int j = 0; j < ncd_y; ++j) {
                int idx_courant = i * ncd_y + j;

                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (di == 0 && dj == 0) continue;

                        int ni = i + di;
                        int nj = j + dj;

                        if (ni >= 0 && ni < ncd_x && nj >= 0 && nj < ncd_y) {
                            int idx_voisine = ni * ncd_y + nj;
                            cellules[idx_courant].addVoisine(&cellules[idx_voisine]);
                        }
                    }
                }
            }
        }
    } 
    else if (dimension == 3) {
        int ncd_y = Ld.getY() / rcut;
        int ncd_z = Ld.getZ() / rcut;
        if (ncd_y == 0) ncd_y = 1;
        if (ncd_z == 0) ncd_z = 1;

        cellules.reserve(ncd_x * ncd_y * ncd_z);
        for (int i = 0; i < ncd_x; ++i) {
            for (int j = 0; j < ncd_y; ++j) {
                for (int k = 0; k < ncd_z; ++k) {
                    Vecteur centre((i + 0.5) * rcut, (j + 0.5) * rcut, (k + 0.5) * rcut);
                    cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(), centre);
                }
            }
        }

        for (int i = 0; i < ncd_x; ++i) {
            for (int j = 0; j < ncd_y; ++j) {
                for (int k = 0; k < ncd_z; ++k) {
                    // Calcul de l'index 1D pour une grille 3D
                    int idx_courant = (i * ncd_y + j) * ncd_z + k;

                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            for (int dk = -1; dk <= 1; ++dk) {
                                if (di == 0 && dj == 0 && dk == 0) continue;

                                int ni = i + di;
                                int nj = j + dj;
                                int nk = k + dk;

                                if (ni >= 0 && ni < ncd_x && 
                                    nj >= 0 && nj < ncd_y && 
                                    nk >= 0 && nk < ncd_z) {
                                    
                                    int idx_voisine = (ni * ncd_y + nj) * ncd_z + nk;
                                    cellules[idx_courant].addVoisine(&cellules[idx_voisine]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Univers::assignerParticulesAuxCellules() {
    for (auto& p : particules) {
        Vecteur pos = p.getPosition();
        int i = pos.getX() / rcut;
        int j = (dimension >= 2) ? (pos.getY() / rcut) : 0;
        int k = (dimension == 3) ? (pos.getZ() / rcut) : 0;

        int ncd_x = Ld.getX() / rcut;
        int ncd_y = (dimension >= 2) ? (Ld.getY() / rcut) : 1;
        int ncd_z = (dimension == 3) ? (Ld.getZ() / rcut) : 1;

        if (i >= 0 && i < ncd_x && j >= 0 && j < ncd_y && k >= 0 && k < ncd_z) {
            int idx_cellule = (i * ncd_y + j) * ncd_z + k;
            cellules[idx_cellule].addParticule(&p);
        }
    }
}