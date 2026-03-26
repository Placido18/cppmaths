#include "Particule.hpp"
#include <iostream>
#include <deque>
#include <random>
#include <cmath>

void calculerForces(std::deque<Particule>& particules){
    ///calculer la force qui s'applique sur chaque particule
    for (size_t i = 0; i < particules.size(); i++){
        Vec2 f = {0.0, 0.0};
        for (size_t j = 0; j < particules.size(); j++){
            if (i != j){
                Vec2 pos_i = particules[i].getPosition();
                Vec2 pos_j = particules[j].getPosition();
                double m_i = particules[i].getMasse();
                double m_j = particules[j].getMasse();
            }
        }
    }

}

// algo de stormer verlet pour le système proposé
int main() {
    
    // Création du système gravitationnel
    std::deque<Particule> systeme;

    systeme.push_back(Particule(1, "Soleil", 1.0, {0.0, 0.0}, {0.0, 0.0}));
    systeme.push_back(Particule(2, "Terre", 3.0e-6, {0.0, 1.0}, {-1.0, 0.0}));
    systeme.push_back(Particule(3, "Jupiter", 9.55e-4, {0.0, 5.36}, {-0.425, 0.0}));
    systeme.push_back(Particule(4, "Halley", 1.0e-14, {34.75, 0.0}, {0.0, 0.0296}));

    // paramètres de l'algorithme
    double dt = 0.015;
    double t_end = 468.5;
    double t = 0.0;

    while (t < t_end){
        t += dt;

        for (auto& i : systeme) {
            Vec2 pos = i.getPosition();
            Vec2 vit = i.getVitesse();
            Vec2 force = i.getForce();
            double m = i.getMasse();

            Vec2 new_pos = {
                pos[0] + dt * (vit[0] + 0.5 / m * force[0] * dt),
                pos[1] + dt * (vit[1] + 0.5 / m * force[1] * dt)
            };
            i.setPosition(new_pos);
        }

        std::deque<Vec2> f_old;
        for (auto& i : systeme) {
            f_old.push_back(i.getForce());
        }

        for (auto& i : systeme){
            Vec2 vit = i.getVitesse();
            Vec2 force = i.getForce();
            double m = i.getMasse();

            Vec2 new_vit = {
                vit[0] + dt * (0.5 / m) * (force[0] + f_old[0]),
                vit[0] + dt * (0.5 / m) * (force[0] + f_old[1])
            };
            i.setVitesse(new_vit);
        }


    return 0;
    }
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