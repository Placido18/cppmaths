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
#include <stdexcept>
#include <algorithm>


/// on utilise deque pour la collection de particules car elle possède les meilleures performances (voir comparaison avec list et vector dand le main)
Univers::Univers(int dimension, int nb_particules, std::deque<Particule> particules, double rcut, Vecteur Ld, std::vector<Cellule> cellules) {
    if (dimension < 1 || dimension > 3)
        throw std::invalid_argument("La dimension doit être 1, 2 ou 3.");
    if (rcut <= 0.0)
        throw std::invalid_argument("Le rayon de coupure rcut doit être strictement positif.");

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

    this->cond_x = ConditionLimite::REFLEXION;
    this->cond_y = ConditionLimite::REFLEXION;
    this->cond_z = ConditionLimite::REFLEXION;

    this->use_champ_gravite = false;
    this->G_champ = 0.0;

    this->Ec_cible = 0.0;

    // Auto-initialisation de la grille si elle est vide
    if (this->cellules.empty()) {
        initialiserMaillage();
        assignerParticulesAuxCellules();
    }
}

void Univers::setPhysicsParams(double eps, double sig, bool gravity, bool lj) {
    if (eps <= 0.0)
        throw std::invalid_argument("Le paramètre epsilon doit être strictement positif.");
    if (sig <= 0.0)
        throw std::invalid_argument("Le paramètre sigma doit être strictement positif.");
    epsilon     = eps;
    sigma       = sig;
    use_gravity = gravity;
    use_LJ      = lj;
}

void Univers::setConditionsLimites(ConditionLimite cx, ConditionLimite cy, ConditionLimite cz) {
    cond_x = cx;
    cond_y = cy;
    cond_z = cz;
}

void Univers::setChampGravite(double G) {
    use_champ_gravite = true;
    G_champ = G;
}

void Univers::setEnergieCinetiqueCible(double Ec) {
    Ec_cible = Ec;
}

void Univers::avancer(double dt) {
    if (dt <= 0.0)
        throw std::invalid_argument("Le pas de temps dt doit être strictement positif.");

    int ncd_x = static_cast<int>(Ld.getX() / rcut);
    if (ncd_x == 0) ncd_x = 1;
    int ncd_y = (dimension >= 2) ? static_cast<int>(Ld.getY() / rcut) : 1;
    if (ncd_y == 0) ncd_y = 1;
    int ncd_z = (dimension == 3) ? static_cast<int>(Ld.getZ() / rcut) : 1;
    if (ncd_z == 0) ncd_z = 1;

    bool has_absorbed = false;

    for (auto& p : particules) {
        // 1. Indices de cellule AVANT le déplacement
        Vecteur ancienne_pos = p.getPosition();
        int i_old = static_cast<int>(ancienne_pos.getX() / rcut);
        int j_old = (dimension >= 2) ? static_cast<int>(ancienne_pos.getY() / rcut) : 0;
        int k_old = (dimension == 3) ? static_cast<int>(ancienne_pos.getZ() / rcut) : 0;

        // 2. Déplacement physique
        p.updatePosition(dt);

        // 3. Application des conditions aux limites
        Vecteur pos = p.getPosition();
        Vecteur vit = p.getVitesse();
        bool absorb = false;

        // --- Direction X ---
        if (cond_x == ConditionLimite::REFLEXION || cond_x == ConditionLimite::REFLEXION_POTENTIEL) {
            if (pos.getX() < 0.0) {
                pos.setX(-pos.getX());
                vit.setX(-vit.getX());
            } else if (pos.getX() >= Ld.getX()) {
                pos.setX(2.0 * Ld.getX() - pos.getX());
                vit.setX(-vit.getX());
            }
        } else if (cond_x == ConditionLimite::PERIODIQUE) {
            if (pos.getX() < 0.0)          pos.setX(pos.getX() + Ld.getX());
            else if (pos.getX() >= Ld.getX()) pos.setX(pos.getX() - Ld.getX());
        } else if (cond_x == ConditionLimite::ABSORPTION) {
            if (pos.getX() < 0.0 || pos.getX() >= Ld.getX()) absorb = true;
        }

        // --- Direction Y ---
        if (dimension >= 2 && !absorb) {
            if (cond_y == ConditionLimite::REFLEXION || cond_y == ConditionLimite::REFLEXION_POTENTIEL) {
                if (pos.getY() < 0.0) {
                    pos.setY(-pos.getY());
                    vit.setY(-vit.getY());
                } else if (pos.getY() >= Ld.getY()) {
                    pos.setY(2.0 * Ld.getY() - pos.getY());
                    vit.setY(-vit.getY());
                }
            } else if (cond_y == ConditionLimite::PERIODIQUE) {
                if (pos.getY() < 0.0)          pos.setY(pos.getY() + Ld.getY());
                else if (pos.getY() >= Ld.getY()) pos.setY(pos.getY() - Ld.getY());
            } else if (cond_y == ConditionLimite::ABSORPTION) {
                if (pos.getY() < 0.0 || pos.getY() >= Ld.getY()) absorb = true;
            }
        }

        // --- Direction Z ---
        if (dimension == 3 && !absorb) {
            if (cond_z == ConditionLimite::REFLEXION || cond_z == ConditionLimite::REFLEXION_POTENTIEL) {
                if (pos.getZ() < 0.0) {
                    pos.setZ(-pos.getZ());
                    vit.setZ(-vit.getZ());
                } else if (pos.getZ() >= Ld.getZ()) {
                    pos.setZ(2.0 * Ld.getZ() - pos.getZ());
                    vit.setZ(-vit.getZ());
                }
            } else if (cond_z == ConditionLimite::PERIODIQUE) {
                if (pos.getZ() < 0.0)          pos.setZ(pos.getZ() + Ld.getZ());
                else if (pos.getZ() >= Ld.getZ()) pos.setZ(pos.getZ() - Ld.getZ());
            } else if (cond_z == ConditionLimite::ABSORPTION) {
                if (pos.getZ() < 0.0 || pos.getZ() >= Ld.getZ()) absorb = true;
            }
        }

        if (absorb) {
            // Sentinelle : position hors domaine → traitée après la boucle
            p.setPosition({-1e10, -1e10, -1e10});
            has_absorbed = true;
            continue; // pas de mise à jour de cellule pour cette particule
        }

        p.setPosition(pos);
        p.setVitesse(vit);

        // 4. Détection du changement de cellule (avec position corrigée)
        int i_new = static_cast<int>(pos.getX() / rcut);
        int j_new = (dimension >= 2) ? static_cast<int>(pos.getY() / rcut) : 0;
        int k_new = (dimension == 3) ? static_cast<int>(pos.getZ() / rcut) : 0;

        if (i_old != i_new || j_old != j_new || k_old != k_new) {
            if (i_old >= 0 && i_old < ncd_x && j_old >= 0 && j_old < ncd_y && k_old >= 0 && k_old < ncd_z) {
                int idx_old = (i_old * ncd_y + j_old) * ncd_z + k_old;
                cellules[idx_old].removeParticule(&p);
            }
            if (i_new >= 0 && i_new < ncd_x && j_new >= 0 && j_new < ncd_y && k_new >= 0 && k_new < ncd_z) {
                int idx_new = (i_new * ncd_y + j_new) * ncd_z + k_new;
                cellules[idx_new].addParticule(&p);
            }
        }
    }

    // 5. Suppression des particules absorbées
    if (has_absorbed) {
        // Vider toutes les cellules avant de modifier la deque (pour éviter les pointeurs pendants)
        for (auto& c : cellules) c.clearParticules();

        particules.erase(
            std::remove_if(particules.begin(), particules.end(),
                [](const Particule& p) { return p.getPosition().getX() < -1e9; }),
            particules.end()
        );
        nb_particules = static_cast<int>(particules.size());

        // Reconstruction des assignations cellule → particule
        assignerParticulesAuxCellules();
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
    if (dt <= 0.0)
        throw std::invalid_argument("Le pas de temps dt doit être strictement positif.");
    if (t_end <= 0.0)
        throw std::invalid_argument("Le temps final t_end doit être strictement positif.");

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

        // 4. Rescaling de l'énergie cinétique (toutes les 1000 itérations)
        if (Ec_cible > 0.0 && iteration % 1000 == 0) {
            double Ec = 0.0;
            for (const auto& p : particules) {
                Vecteur v = p.getVitesse();
                Ec += 0.5 * p.getMasse() * (v.getX()*v.getX() + v.getY()*v.getY() + v.getZ()*v.getZ());
            }
            if (Ec > 0.0) {
                double beta = std::sqrt(Ec_cible / Ec);
                for (auto& p : particules) {
                    Vecteur v = p.getVitesse();
                    p.setVitesse({v.getX()*beta, v.getY()*beta, v.getZ()*beta});
                }
            }
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

    // Champ gravitationnel uniforme externe : F^G_i = (0, m_i * G, 0)
    if (use_champ_gravite) {
        for (auto& p : particules) {
            p.ajouterForce({0.0, p.getMasse() * G_champ, 0.0});
        }
    }

    // Potentiel de paroi pour la réflexion par potentiel (Q3)
    // F_i = -24ε * (1/2r) * (σ/2r)^6 * (1 - 2*(σ/2r)^6), avec r_cut_wall = 2^(1/6)*σ
    const bool has_paroi_pot = (cond_x == ConditionLimite::REFLEXION_POTENTIEL ||
                                cond_y == ConditionLimite::REFLEXION_POTENTIEL ||
                                cond_z == ConditionLimite::REFLEXION_POTENTIEL);
    if (has_paroi_pot) {
        const double rcut_wall = std::pow(2.0, 1.0/6.0) * sigma;

        // Force exercée par un mur à distance r (positive = répulsion depuis le mur)
        auto forceParoi = [&](double r) -> double {
            if (r <= 0.0 || r >= rcut_wall) return 0.0;
            double sr6 = std::pow(sigma / (2.0 * r), 6.0);
            return -24.0 * epsilon / (2.0 * r) * sr6 * (1.0 - 2.0 * sr6);
        };

        for (auto& p : particules) {
            Vecteur pos = p.getPosition();

            if (cond_x == ConditionLimite::REFLEXION_POTENTIEL) {
                double fx = forceParoi(pos.getX());                       // paroi gauche  (x=0)  → +x
                double fx_r = forceParoi(Ld.getX() - pos.getX());         // paroi droite  (x=Lx) → -x
                p.ajouterForce({fx - fx_r, 0.0, 0.0});
            }
            if (dimension >= 2 && cond_y == ConditionLimite::REFLEXION_POTENTIEL) {
                double fy = forceParoi(pos.getY());                       // paroi basse   (y=0)  → +y
                double fy_h = forceParoi(Ld.getY() - pos.getY());         // paroi haute   (y=Ly) → -y
                p.ajouterForce({0.0, fy - fy_h, 0.0});
            }
            if (dimension == 3 && cond_z == ConditionLimite::REFLEXION_POTENTIEL) {
                double fz = forceParoi(pos.getZ());                       // paroi avant   (z=0)  → +z
                double fz_b = forceParoi(Ld.getZ() - pos.getZ());         // paroi arrière (z=Lz) → -z
                p.ajouterForce({0.0, 0.0, fz - fz_b});
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
