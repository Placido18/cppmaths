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

// ---------------------------------------------------------------------------
// Helpers locaux (pas d'état de classe requis)
// ---------------------------------------------------------------------------

/// Applique la condition aux limites sur un axe ; renvoie true si absorbé.
static bool appliquerConditionLimite1D(double& pos, double& vit, double Lmax, ConditionLimite cond) {
    if (cond == ConditionLimite::REFLEXION || cond == ConditionLimite::REFLEXION_POTENTIEL) {
        if (pos < 0.0)        { pos = -pos;             vit = -vit; }
        else if (pos >= Lmax) { pos = 2.0*Lmax - pos;   vit = -vit; }
    } else if (cond == ConditionLimite::PERIODIQUE) {
        if (pos < 0.0)        pos += Lmax;
        else if (pos >= Lmax) pos -= Lmax;
    } else if (cond == ConditionLimite::ABSORPTION) {
        if (pos < 0.0 || pos >= Lmax) return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Constructeur
// ---------------------------------------------------------------------------

/// on utilise deque pour la collection de particules car elle possède les meilleures performances
Univers::Univers(int dimension, int nb_particules, std::deque<Particule> particules,
                 double rcut, Vecteur Ld, std::vector<Cellule> cellules) {
    if (dimension < 1 || dimension > 3)
        throw std::invalid_argument("La dimension doit être 1, 2 ou 3.");
    if (rcut <= 0.0)
        throw std::invalid_argument("Le rayon de coupure rcut doit être strictement positif.");

    this->dimension      = dimension;
    this->nb_particules  = nb_particules;
    this->particules     = particules;
    this->rcut           = rcut;
    this->Ld             = Ld;
    this->cellules       = cellules;

    this->epsilon         = 5.0;
    this->sigma           = 1.0;
    this->use_gravity     = false;
    this->use_LJ          = true;

    this->cond_x          = ConditionLimite::REFLEXION;
    this->cond_y          = ConditionLimite::REFLEXION;
    this->cond_z          = ConditionLimite::REFLEXION;

    this->use_champ_gravite = false;
    this->G_champ           = 0.0;
    this->Ec_cible          = 0.0;

    if (this->cellules.empty()) {
        initialiserMaillage();
        assignerParticulesAuxCellules();
    }
}

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Méthodes privées
// ---------------------------------------------------------------------------

std::array<int, 3> Univers::dimensionsMaillage() const {
    int nx = std::max(1, static_cast<int>(Ld.getX() / rcut));
    int ny = (dimension >= 2) ? std::max(1, static_cast<int>(Ld.getY() / rcut)) : 1;
    int nz = (dimension == 3) ? std::max(1, static_cast<int>(Ld.getZ() / rcut)) : 1;
    return {nx, ny, nz};
}

double Univers::calculerEnergieCinetique() const {
    double Ec = 0.0;
    for (const auto& p : particules) {
        Vecteur v = p.getVitesse();
        Ec += 0.5 * p.getMasse() * (v.getX()*v.getX() + v.getY()*v.getY() + v.getZ()*v.getZ());
    }
    return Ec;
}

void Univers::rescalerVitesses() {
    double Ec = calculerEnergieCinetique();
    if (Ec <= 0.0) return;
    double beta = std::sqrt(Ec_cible / Ec);
    for (auto& p : particules) {
        Vecteur v = p.getVitesse();
        p.setVitesse({v.getX()*beta, v.getY()*beta, v.getZ()*beta});
    }
}

void Univers::appliquerForcesExternes() {
    // Champ gravitationnel uniforme : F^G_i = (0, m_i * G, 0)
    if (use_champ_gravite) {
        for (auto& p : particules)
            p.ajouterForce({0.0, p.getMasse() * G_champ, 0.0});
    }

    // Potentiel de paroi (Q3) : F_i = -24ε/(2r) * (σ/2r)^6 * (1 - 2*(σ/2r)^6)
    const bool paroi_x = (cond_x == ConditionLimite::REFLEXION_POTENTIEL);
    const bool paroi_y = (dimension >= 2) && (cond_y == ConditionLimite::REFLEXION_POTENTIEL);
    const bool paroi_z = (dimension == 3) && (cond_z == ConditionLimite::REFLEXION_POTENTIEL);
    if (!paroi_x && !paroi_y && !paroi_z) return;

    const double rcut_wall = std::pow(2.0, 1.0/6.0) * sigma;
    auto forceParoi = [&](double r) -> double {
        if (r <= 0.0 || r >= rcut_wall) return 0.0;
        double sr6 = std::pow(sigma / (2.0 * r), 6.0);
        return -24.0 * epsilon / (2.0 * r) * sr6 * (1.0 - 2.0 * sr6);
    };

    for (auto& p : particules) {
        Vecteur pos = p.getPosition();
        if (paroi_x)
            p.ajouterForce({forceParoi(pos.getX()) - forceParoi(Ld.getX() - pos.getX()), 0.0, 0.0});
        if (paroi_y)
            p.ajouterForce({0.0, forceParoi(pos.getY()) - forceParoi(Ld.getY() - pos.getY()), 0.0});
        if (paroi_z)
            p.ajouterForce({0.0, 0.0, forceParoi(pos.getZ()) - forceParoi(Ld.getZ() - pos.getZ())});
    }
}

// ---------------------------------------------------------------------------
// Simulation
// ---------------------------------------------------------------------------

void Univers::avancer(double dt) {
    if (dt <= 0.0)
        throw std::invalid_argument("Le pas de temps dt doit être strictement positif.");

    auto [ncd_x, ncd_y, ncd_z] = dimensionsMaillage();
    bool has_absorbed = false;

    for (auto& p : particules) {
        // 1. Indices de cellule AVANT le déplacement
        Vecteur ancienne_pos = p.getPosition();
        int i_old = static_cast<int>(ancienne_pos.getX() / rcut);
        int j_old = (dimension >= 2) ? static_cast<int>(ancienne_pos.getY() / rcut) : 0;
        int k_old = (dimension == 3) ? static_cast<int>(ancienne_pos.getZ() / rcut) : 0;

        // 2. Déplacement physique
        p.updatePosition(dt);

        // 3. Conditions aux limites (une méthode par axe via le helper statique)
        Vecteur pos = p.getPosition();
        Vecteur vit = p.getVitesse();
        double px = pos.getX(), vx = vit.getX();
        double py = pos.getY(), vy = vit.getY();
        double pz = pos.getZ(), vz = vit.getZ();

        bool absorb = appliquerConditionLimite1D(px, vx, Ld.getX(), cond_x);
        if (!absorb && dimension >= 2)
            absorb = appliquerConditionLimite1D(py, vy, Ld.getY(), cond_y);
        if (!absorb && dimension == 3)
            absorb = appliquerConditionLimite1D(pz, vz, Ld.getZ(), cond_z);

        if (absorb) {
            p.setPosition({-1e10, -1e10, -1e10}); // sentinelle pour la suppression
            has_absorbed = true;
            continue;
        }

        p.setPosition({px, py, pz});
        p.setVitesse({vx, vy, vz});

        // 4. Mise à jour des cellules si changement de cellule
        int i_new = static_cast<int>(px / rcut);
        int j_new = (dimension >= 2) ? static_cast<int>(py / rcut) : 0;
        int k_new = (dimension == 3) ? static_cast<int>(pz / rcut) : 0;

        if (i_old != i_new || j_old != j_new || k_old != k_new) {
            if (i_old >= 0 && i_old < ncd_x && j_old >= 0 && j_old < ncd_y && k_old >= 0 && k_old < ncd_z)
                cellules[(i_old * ncd_y + j_old) * ncd_z + k_old].removeParticule(&p);
            if (i_new >= 0 && i_new < ncd_x && j_new >= 0 && j_new < ncd_y && k_new >= 0 && k_new < ncd_z)
                cellules[(i_new * ncd_y + j_new) * ncd_z + k_new].addParticule(&p);
        }
    }

    // 5. Suppression des particules absorbées (rebuild pour éviter les pointeurs pendants)
    if (has_absorbed) {
        for (auto& c : cellules) c.clearParticules();
        particules.erase(
            std::remove_if(particules.begin(), particules.end(),
                [](const Particule& p) { return p.getPosition().getX() < -1e9; }),
            particules.end()
        );
        nb_particules = static_cast<int>(particules.size());
        assignerParticulesAuxCellules();
    }
}

void Univers::calculerForces() {
    for (auto& p : particules)
        p.setForce({0.0, 0.0, 0.0});

    // Forces inter-particules (gravité newtonienne et/ou Lennard-Jones)
    auto applyPair = [&](Particule& pi, Particule& pj) {
        Vecteur r_ij  = pj.getPosition() - pi.getPosition();
        double dist_sq = r_ij.getX()*r_ij.getX() + r_ij.getY()*r_ij.getY() + r_ij.getZ()*r_ij.getZ();
        if (dist_sq <= 0.0 || dist_sq > rcut*rcut) return;
        double dist = std::sqrt(dist_sq);

        double facteur = 0.0;
        if (use_gravity)
            facteur += (pi.getMasse() * pj.getMasse()) / (dist * dist_sq);
        if (use_LJ) {
            double sr6 = std::pow(sigma / dist, 6.0);
            facteur += 24.0 * epsilon / dist_sq * sr6 * (1.0 - 2.0*sr6);
        }

        Vecteur f = r_ij * facteur;
        pi.ajouterForce(f);
        pj.ajouterForce(f * (-1.0));
    };

    // Parcours par demi-coquille (chaque paire traitée une seule fois)
    for (Cellule& cell : cellules) {
        const auto& parts = cell.getParticules();
        for (size_t a = 0; a < parts.size(); ++a)
            for (size_t b = a + 1; b < parts.size(); ++b)
                applyPair(*parts[a], *parts[b]);

        for (Cellule* neigh : cell.getVoisines()) {
            if (neigh > &cell) {
                for (Particule* pi : parts)
                    for (Particule* pj : neigh->getParticules())
                        applyPair(*pi, *pj);
            }
        }
    }

    // Forces externes : champ gravitationnel + potentiel de paroi
    appliquerForcesExternes();
}

void Univers::evoluer(double dt, double t_end) {
    double t = 0.0;
    while (t < t_end) {
        calculerForces();
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

    std::ofstream fichier;
    if (particules.size() <= 100)
        fichier.open("positions.txt");

    calculerForces();
    sauvegarderVTK(frame_vtk++);

    while (t < t_end) {
        t += dt;
        ++iteration;

        avancer(dt);

        if (fichier.is_open()) {
            fichier << t << " ";
            for (const auto& p : particules)
                fichier << p.getPosition().getX() << " "
                        << p.getPosition().getY() << " "
                        << p.getPosition().getZ() << " ";
            fichier << "\n";
        }

        // Sauvegarde des forces avant recalcul, puis mise à jour des vitesses
        std::deque<Vecteur> f_old;
        for (const auto& p : particules) f_old.push_back(p.getForce());
        calculerForces();
        for (size_t i = 0; i < particules.size(); ++i)
            particules[i].updateVitesse(dt, f_old[i]);

        // Rescaling toutes les 1000 itérations si une énergie cible est définie
        if (Ec_cible > 0.0 && iteration % 1000 == 0)
            rescalerVitesses();

        if (iteration % vtk_freq == 0)
            sauvegarderVTK(frame_vtk++);
    }
}

// ---------------------------------------------------------------------------
// Utilitaires
// ---------------------------------------------------------------------------

void Univers::appliquerVitesse(double vitesse) {
    for (auto& p : particules) {
        Vecteur vit = p.getVitesse();
        double norm = std::sqrt(vit.getX()*vit.getX() + vit.getY()*vit.getY() + vit.getZ()*vit.getZ());
        if (norm > 0) {
            double scale = vitesse / norm;
            p.setVitesse({vit.getX()*scale, vit.getY()*scale, vit.getZ()*scale});
        }
    }
}

void Univers::afficher(double t) const {
    std::cout << "Temps: " << t << "s\n";
    for (const auto& p : particules) {
        std::cout << "Particule " << p.getId() << " (" << p.getType() << "): Position = ("
                  << p.getPosition().getX();
        if (dimension >= 2) std::cout << ", " << p.getPosition().getY();
        if (dimension == 3) std::cout << ", " << p.getPosition().getZ();
        std::cout << ")\n";
    }
}

void Univers::sauvegarderVTK(int iteration) const {
    std::ostringstream nom_fichier;
    nom_fichier << "output_" << std::setfill('0') << std::setw(4) << iteration << ".vtu";
    std::ofstream out(nom_fichier.str());
    if (!out.is_open()) return;

    out << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n"
        << "  <UnstructuredGrid>\n"
        << "    <Piece NumberOfPoints=\"" << particules.size() << "\" NumberOfCells=\"0\">\n";

    out << "      <Points>\n"
        << "        <DataArray name=\"Position\" type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (const auto& p : particules)
        out << "          " << p.getPosition().getX() << " "
            << p.getPosition().getY() << " " << p.getPosition().getZ() << "\n";
    out << "        </DataArray>\n      </Points>\n";

    out << "      <PointData Vectors=\"Velocity\">\n"
        << "        <DataArray type=\"Float32\" Name=\"Velocity\" NumberOfComponents=\"3\" format=\"ascii\">\n";
    for (const auto& p : particules)
        out << "          " << p.getVitesse().getX() << " "
            << p.getVitesse().getY() << " " << p.getVitesse().getZ() << "\n";
    out << "        </DataArray>\n"
        << "        <DataArray type=\"Float32\" Name=\"Masse\" format=\"ascii\">\n";
    for (const auto& p : particules)
        out << "          " << p.getMasse() << "\n";
    out << "        </DataArray>\n"
        << "        <DataArray type=\"Int32\" Name=\"Groupe\" format=\"ascii\">\n";
    for (const auto& p : particules)
        out << "          " << (p.getType() == "rouge" ? 1 : 0) << "\n";
    out << "        </DataArray>\n      </PointData>\n";

    out << "      <Cells>\n"
        << "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n        </DataArray>\n"
        << "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n        </DataArray>\n"
        << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n        </DataArray>\n"
        << "      </Cells>\n    </Piece>\n  </UnstructuredGrid>\n</VTKFile>\n";
}

int Univers::getDimension() const { return dimension; }

const std::deque<Particule>& Univers::getParticules() const { return particules; }

// ---------------------------------------------------------------------------
// Maillage
// ---------------------------------------------------------------------------

void Univers::initialiserMaillage() {
    auto [ncd_x, ncd_y, ncd_z] = dimensionsMaillage();

    if (dimension == 1) {
        cellules.reserve(ncd_x);
        for (int i = 0; i < ncd_x; ++i)
            cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(),
                                  Vecteur((i + 0.5) * rcut, 0, 0));
        for (int i = 0; i < ncd_x; ++i)
            for (int di = -1; di <= 1; ++di) {
                if (di == 0) continue;
                int ni = i + di;
                if (ni >= 0 && ni < ncd_x) cellules[i].addVoisine(&cellules[ni]);
            }
    }
    else if (dimension == 2) {
        cellules.reserve(ncd_x * ncd_y);
        for (int i = 0; i < ncd_x; ++i)
            for (int j = 0; j < ncd_y; ++j)
                cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(),
                                      Vecteur((i + 0.5)*rcut, (j + 0.5)*rcut, 0));

        for (int i = 0; i < ncd_x; ++i)
            for (int j = 0; j < ncd_y; ++j)
                for (int di = -1; di <= 1; ++di)
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (di == 0 && dj == 0) continue;
                        int ni = i + di, nj = j + dj;
                        if (ni >= 0 && ni < ncd_x && nj >= 0 && nj < ncd_y)
                            cellules[i*ncd_y + j].addVoisine(&cellules[ni*ncd_y + nj]);
                    }
    }
    else { // dimension == 3
        cellules.reserve(ncd_x * ncd_y * ncd_z);
        for (int i = 0; i < ncd_x; ++i)
            for (int j = 0; j < ncd_y; ++j)
                for (int k = 0; k < ncd_z; ++k)
                    cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(),
                                          Vecteur((i+0.5)*rcut, (j+0.5)*rcut, (k+0.5)*rcut));

        for (int i = 0; i < ncd_x; ++i)
            for (int j = 0; j < ncd_y; ++j)
                for (int k = 0; k < ncd_z; ++k)
                    for (int di = -1; di <= 1; ++di)
                        for (int dj = -1; dj <= 1; ++dj)
                            for (int dk = -1; dk <= 1; ++dk) {
                                if (di == 0 && dj == 0 && dk == 0) continue;
                                int ni = i+di, nj = j+dj, nk = k+dk;
                                if (ni >= 0 && ni < ncd_x && nj >= 0 && nj < ncd_y && nk >= 0 && nk < ncd_z)
                                    cellules[(i*ncd_y+j)*ncd_z+k].addVoisine(
                                        &cellules[(ni*ncd_y+nj)*ncd_z+nk]);
                            }
    }
}

void Univers::assignerParticulesAuxCellules() {
    auto [ncd_x, ncd_y, ncd_z] = dimensionsMaillage();
    for (auto& p : particules) {
        Vecteur pos = p.getPosition();
        int i = static_cast<int>(pos.getX() / rcut);
        int j = (dimension >= 2) ? static_cast<int>(pos.getY() / rcut) : 0;
        int k = (dimension == 3) ? static_cast<int>(pos.getZ() / rcut) : 0;
        if (i >= 0 && i < ncd_x && j >= 0 && j < ncd_y && k >= 0 && k < ncd_z)
            cellules[(i*ncd_y + j)*ncd_z + k].addParticule(&p);
    }
}
