/**
 * @file Univers.cpp
 * @brief Implémentation de la classe Univers et de la simulation physique.
 */

#include "Univers.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>


/// on utilise deque pour la collection de particules car elle possède les meilleures performances (voir comparaison avec list et vector dand le main)
Univers::Univers(int dimension, int nb_particules, std::deque<Particule> particules, double rcut, Vecteur Ld, std::vector<Cellule> cellules) {
    this->dimension = dimension;
    this->nb_particules = nb_particules;
    this->particules = particules;
    this->rcut = rcut;
    this->Ld = Ld;
    this->cellules = cellules;

    this->epsilon    = 5.0;
    this->sigma      = 1.0;
    this->use_gravity = false;
    this->use_LJ     = true;

    // Auto-initialisation de la grille si elle est vide
    if (this->cellules.empty()) {
        initialiserMaillage();
        assignerParticulesAuxCellules();
    }
}

void Univers::setPhysicsParams(double eps, double sig, bool gravity, bool lj) {
    epsilon     = eps;
    sigma       = sig;
    use_gravity = gravity;
    use_LJ      = lj;
}

void Univers::avancer(double dt) {
    // Calcul des dimensions de la grille (limites)
    int ncd_x = Ld.getX() / rcut;
    if (ncd_x == 0) ncd_x = 1;
    int ncd_y = (dimension >= 2) ? (int)(Ld.getY() / rcut) : 1;
    if (ncd_y == 0) ncd_y = 1;
    int ncd_z = (dimension == 3) ? (int)(Ld.getZ() / rcut) : 1;
    if (ncd_z == 0) ncd_z = 1;

    for (auto& p : particules) {
        // 1. Calcul des indices de cellule AVANT le déplacement
        Vecteur ancienne_pos = p.getPosition();
        int i_old = ancienne_pos.getX() / rcut;
        int j_old = (dimension >= 2) ? (int)(ancienne_pos.getY() / rcut) : 0;
        int k_old = (dimension == 3) ? (int)(ancienne_pos.getZ() / rcut) : 0;

        // 2. Déplacement physique de la particule
        p.updatePosition(dt);

        // 3. Calcul des indices de cellule APRES le déplacement
        Vecteur nouvelle_pos = p.getPosition();
        int i_new = nouvelle_pos.getX() / rcut;
        int j_new = (dimension >= 2) ? (int)(nouvelle_pos.getY() / rcut) : 0;
        int k_new = (dimension == 3) ? (int)(nouvelle_pos.getZ() / rcut) : 0;

        // 4. Détection du changement de cellule
        if (i_old != i_new || j_old != j_new || k_old != k_new) {
            
            // Retrait de l'ancienne cellule (si elle était bien dans le domaine spatial)
            if (i_old >= 0 && i_old < ncd_x && j_old >= 0 && j_old < ncd_y && k_old >= 0 && k_old < ncd_z) {
                int idx_old = (i_old * ncd_y + j_old) * ncd_z + k_old;
                cellules[idx_old].removeParticule(&p);
            }

            // Ajout à la nouvelle cellule (si elle ne sort pas des limites du domaine spatial)
            if (i_new >= 0 && i_new < ncd_x && j_new >= 0 && j_new < ncd_y && k_new >= 0 && k_new < ncd_z) {
                int idx_new = (i_new * ncd_y + j_new) * ncd_z + k_new;
                cellules[idx_new].addParticule(&p);
            }
        }
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

void Univers::evoluerVerlet(double dt, double t_end, int vtk_freq) {
    double t = 0.0;
    int iteration = 0;
    int frame_vtk = 0;
    // positions.txt : uniquement pour les petits systèmes (gravitationnel)
    std::ofstream fichier;
    if (particules.size() <= 100) {
        fichier.open("positions.txt");
    }
    calculerForces();

    sauvegarderVTK(frame_vtk++);

    while (t < t_end) {
        t += dt;
        iteration++;

        // 1. Mise à jour des positions
        avancer(dt);

        if (fichier.is_open()) {
            fichier << t << " ";
            for (const auto& p : particules) {
                fichier << p.getPosition().getX() << " "
                        << p.getPosition().getY() << " "
                        << p.getPosition().getZ() << " ";
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
        
        if (iteration % vtk_freq == 0) {
            sauvegarderVTK(frame_vtk++);
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

void Univers::sauvegarderVTK(int iteration) const {
    std::ostringstream nom_fichier;
    nom_fichier << "output_" << std::setfill('0') << std::setw(4) << iteration << ".vtu";
    std::ofstream out(nom_fichier.str());

    if (!out.is_open()) return;

    out << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n";
    out << "  <UnstructuredGrid>\n";
    out << "    <Piece NumberOfPoints=\"" << particules.size() << "\" NumberOfCells=\"0\">\n";
    
    out << "      <Points>\n";
    out << "        <DataArray name=\"Position\" type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (const auto& p : particules) {
        out << "          " << p.getPosition().getX() << " " << p.getPosition().getY() << " " << p.getPosition().getZ() << "\n";
    }
    out << "        </DataArray>\n";
    out << "      </Points>\n";
    
    out << "      <PointData Vectors=\"Velocity\">\n";
    out << "        <DataArray type=\"Float32\" Name=\"Velocity\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (const auto& p : particules) {
        out << "          " << p.getVitesse().getX() << " " << p.getVitesse().getY() << " " << p.getVitesse().getZ() << "\n";
    }
    out << "        </DataArray>\n";
    out << "        <DataArray type=\"Float32\" Name=\"Masse\" format=\"ascii\">\n";
    for (const auto& p : particules) {
        out << "          " << p.getMasse() << "\n";
    }
    out << "        </DataArray>\n";
    out << "        <DataArray type=\"Int32\" Name=\"Groupe\" format=\"ascii\">\n";
    for (const auto& p : particules) {
        out << "          " << (p.getType() == "rouge" ? 1 : 0) << "\n";
    }
    out << "        </DataArray>\n";
    out << "      </PointData>\n";
    
    out << "      <Cells>\n";
    out << "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n        </DataArray>\n";
    out << "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n        </DataArray>\n";
    out << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n        </DataArray>\n";
    out << "      </Cells>\n";
    out << "    </Piece>\n";
    out << "  </UnstructuredGrid>\n";
    out << "</VTKFile>\n";
}

void Univers::calculerForces() {
    for (auto& p : particules) {
        p.setForce({0.0, 0.0, 0.0});
    }

    // Calcule et applique la force entre deux particules (action-réaction)
    auto applyPair = [&](Particule& pi, Particule& pj) {
        Vecteur r_ij = pj.getPosition() - pi.getPosition();
        double dist_sq = r_ij.getX()*r_ij.getX() + r_ij.getY()*r_ij.getY() + r_ij.getZ()*r_ij.getZ();
        if (dist_sq <= 0.0 || dist_sq > rcut*rcut) return;
        double dist = std::sqrt(dist_sq);

        double facteur = 0.0;

        if (use_gravity) {
            facteur += (pi.getMasse() * pj.getMasse()) / (dist * dist_sq);
        }

        if (use_LJ) {
            double sr  = sigma / dist;
            double sr6 = sr*sr*sr*sr*sr*sr;
            facteur += 24.0 * epsilon / dist_sq * sr6 * (1.0 - 2.0*sr6);
        }

        Vecteur f = r_ij * facteur;
        pi.ajouterForce(f);
        pj.ajouterForce(f * (-1.0));
    };

    // Parcours des cellules : paires intra-cellule et paires avec voisines (demi-coquille)
    for (Cellule& cell : cellules) {
        const auto& parts = cell.getParticules();

        for (size_t a = 0; a < parts.size(); ++a) {
            for (size_t b = a + 1; b < parts.size(); ++b) {
                applyPair(*parts[a], *parts[b]);
            }
        }

        // On ne traite chaque paire de cellules voisines qu'une seule fois
        for (Cellule* neigh : cell.getVoisines()) {
            if (neigh > &cell) {
                for (Particule* pi : parts) {
                    for (Particule* pj : neigh->getParticules()) {
                        applyPair(*pi, *pj);
                    }
                }
            }
        }
    }
}

int Univers::getDimension() const {
    return dimension;

}

const std::deque<Particule>& Univers::getParticules() const {
    return particules;
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
        if (ncd_x == 0) ncd_x = 1;
        int ncd_y = (dimension >= 2) ? (Ld.getY() / rcut) : 1;
        if (ncd_y == 0) ncd_y = 1;
        int ncd_z = (dimension == 3) ? (Ld.getZ() / rcut) : 1;
        if (ncd_z == 0) ncd_z = 1;

        if (i >= 0 && i < ncd_x && j >= 0 && j < ncd_y && k >= 0 && k < ncd_z) {
            int idx_cellule = (i * ncd_y + j) * ncd_z + k;
            cellules[idx_cellule].addParticule(&p);
        }
    }
}