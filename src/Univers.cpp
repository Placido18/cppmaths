#include "Univers.hpp"
#include <iostream>
#include <cmath>

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
    for (size_t i = 0; i < particules.size() - 1; ++i) {
        for (size_t j = i + 1; j < particules.size(); ++j) {
            Vecteur pos_i = particules[i].getPosition();
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
