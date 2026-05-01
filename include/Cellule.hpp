#ifndef CELLULE_HPP
#define CELLULE_HPP

#include "Particule.hpp"
#include "Vecteur.hpp"
#include <vector>

/**
 * @brief Représente une cellule de l'espace pour le partitionnement du maillage.
 * 
 * Utilisée pour optimiser le calcul des forces en restreignant les interactions
 * aux particules des cellules voisines.
 */
class Cellule{
    private:
        std::vector<Particule*> particules;//liste de pointeurs vers les particules présentes dans la cellule
        //on utilise des pointeurs pour éviter de copier les particules 
        std::vector<Cellule*> voisines; // pointeurs vers les cellules voisines
        //comme les voisins sont fixés, on pourrait utiliser:
        //std::array<Cellule*, dimension*dimension*dimension> voisines; check
        Vecteur centre; // position du centre de la cellule, utile pour calculer les forces d'interaction

    public:
        /**
         * @brief Constructeur d'une Cellule.
         * @param particules Liste des pointeurs vers les particules contenues.
         * @param voisines Liste des pointeurs vers les cellules voisines.
         * @param centre Vecteur représentant le centre géométrique de la cellule.
         */
        Cellule(std::vector<Particule*> particules, std::vector<Cellule*> voisines, Vecteur centre);        
        /**
         * @brief Ajoute une cellule voisine à la liste des voisines de l'instance.
         * @param cellule Pointeur vers la cellule voisine à ajouter.
         */
        void addVoisine(Cellule* cellule);

        /**
         * @brief Ajoute une particule à cette cellule.
         * @param p Pointeur vers la particule à ajouter.
         */
        void addParticule(Particule* p);

        /**
         * @brief Récupère la liste des cellules voisines.
         * @return Un vecteur de pointeurs vers les cellules voisines.
         */
        std::vector<Cellule*> getVoisines() const;

        /**
         * @brief Récupère la liste des particules présentes dans cette cellule.
         * @return Un vecteur de pointeurs vers les particules contenues.
         */
        std::vector<Particule*> getParticules() const;

        /**
         * @brief Récupère le centre de la cellule.
         * @return Un vecteur représentant le centre géométrique de la cellule.
         */
        Vecteur getCentre() const { return centre; }

        /**
         * @brief Retire une particule de cette cellule.
         * @param p Pointeur vers la particule à retirer.
         */void removeParticule(Particule* p);

};


#endif