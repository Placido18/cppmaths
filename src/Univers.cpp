#include "Univers.hpp"
#include <iostream>

/// on utilise deque pour la collection de particules car elle possède les meilleures performances (voir comparaison avec list et vector dand le main)
Univers::Univers(int dimension, int nb_particules, std::deque<Particule> particules) {
    this->dimension = dimension;
    this->nb_particules = nb_particules;
    this->particules = particules;
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