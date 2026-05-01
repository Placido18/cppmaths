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
    //Pour chaque direction on a ncd cellules avec :
    int ncd_x = Ld.getX() / rcut;

    if (dimension == 1) {
        for (int i = 0; i < ncd_x; ++i) {
            //ajout du centre de la cellule dans le constructeur de la cellule
            Vecteur centre((i + 0.5) * rcut, 0, 0);
            //ajout de la cellule dans le maillage de l'univers
            cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(), centre);
            //ajout de la cellule dans la liste des voisines de la cellule précédente
            if (i > 0) {
                cellules[i].addVoisine(&cellules[i - 1]);
                cellules[i - 1].addVoisine(&cellules[i]);
            }  
        }
    }

    if (dimension == 2) {
        // les cellules sont organisées en grille, on les stocke dans un vecteur, la cellule d'indice (i,j) est stockée à l'indice i*ncd_y + j du vecteur
        int ncd_y = Ld.getY() / rcut;
        for (int i = 0; i < ncd_x; ++i) {
            for (int j = 0; j < ncd_y; ++j) {
                Vecteur centre((i + 0.5) * rcut, (j + 0.5) * rcut, 0);
                cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(), centre);
                if (j>0 && i>0) {
                    cellules[i * ncd_y + j].addVoisine(&cellules[i * ncd_y + j - 1]);
                    cellules[i * ncd_y + j - 1].addVoisine(&cellules[i * ncd_y + j]);
                    cellules[i * ncd_y + j].addVoisine(&cellules[(i - 1) * ncd_y + j]);
                    cellules[(i - 1) * ncd_y + j].addVoisine(&cellules[i * ncd_y + j]);
                    cellules[i * ncd_y + j].addVoisine(&cellules[(i - 1) * ncd_y + j - 1]);
                    cellules[(i - 1) * ncd_y + j - 1].addVoisine(&cellules[i * ncd_y + j]);
                    if (j<ncd_y-1) {
                        cellules[i * ncd_y + j].addVoisine(&cellules[(i - 1) * ncd_y + j + 1]);
                        cellules[(i - 1) * ncd_y + j + 1].addVoisine(&cellules[i * ncd_y + j]);
                    }
                    if (i<ncd_x-1){
                        cellules[i * ncd_y + j].addVoisine(&cellules[(i + 1) * ncd_y + j-1]);
                        cellules[(i + 1) * ncd_y + j-1].addVoisine(&cellules[i * ncd_y + j]);
                    }
                }
            }
        }
    }
    if (dimension == 3) {
        int ncd_y = Ld.getY() / rcut;
        int ncd_z = Ld.getZ() / rcut;
        for (int i = 0; i < ncd_x; ++i) {
            for (int j = 0; j < ncd_y; ++j) {
                for (int k = 0; k < ncd_z; ++k) {
                    Vecteur centre((i + 0.5) * rcut, (j + 0.5) * rcut, (k + 0.5) * rcut);
                    cellules.emplace_back(std::vector<Particule*>(), std::vector<Cellule*>(), centre);
                }
            }
        }
    }
}