#include "Univers.hpp"
/// on utilise deque pour la collection de particules car elle possède les meilleures performances (voir comparaison avec list et vector dand le main)
Univers::Univers(int dimension, int nb_particules, std::deque<Particule> particules) {
    this->dimension = dimension;
    this->nb_particules = nb_particules;
    this->particules = particules;
}