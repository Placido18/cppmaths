#ifndef UNIVERS_HPP
#define UNIVERS_HPP
#include <deque>
#include "Particule.hpp"
#include "Vecteur.hpp"
#include "Cellule.hpp"

#include <vector>

/**
 * @brief Classe représentant l'univers de la simulation.
 * 
 * Gère l'ensemble des particules, le partitionnement spatial (maillage en cellules), et l'évolution temporelle du système.
 */
class Univers {
    private:
        int dimension;
        int nb_particules;

        //les particules sont stockées dans l'univers,
        //les cellules contiennent des pointeurs vers les particules, mais ne les possèdent pas
        //voir s'il n'y a pas de problème de cohérence entre deque et vector, check
        std::deque<Particule> particules;

        //ajout du rayon de coupe et d'une longueur caractéristique

        double rcut;// c'est à la fois la distance de coupure pour les interactions et donc la taille des cellules du maillage
        Vecteur Ld; 

        //un univers possède une liste de cellules
        std::vector<Cellule> cellules;
        //std::array<Cellule, dimension*dimension*dimension> cellules; check
        
        
    public:
        /**
         * @brief Constructeur de l'Univers.
         * @param dimension Dimension de l'espace (1, 2 ou 3).
         * @param nb_particules Nombre de particules dans l'univers.
         * @param particules Conteneur contenant toutes les particules.
         * @param rcut Rayon de coupure pour les interactions et taille des cellules.
         * @param Ld Dimensions géométriques de l'univers.
         * @param cellules Liste des cellules formant le maillage.
         */
        Univers(int dimension, int nb_particules, std::deque<Particule> particules, double rcut, Vecteur Ld, std::vector<Cellule> cellules);

        /**
         * @brief Fait avancer les particules pendant un pas de temps.
         * @param dt Pas de temps de la simulation.
         */
        void avancer(double dt);

        /**
         * @brief Affiche les positions des particules.
         * @param t Instant actuel de la simulation.
         */
        void afficher(double t) const;

        /**
         * @brief Calcule les forces d'interaction entre les particules.
         */
        void calculerForces() ;

        /**
         * @brief Fait évoluer le système complet sur une durée donnée.
         * @param dt Pas de temps d'itération.
         * @param t_end Temps final d'arrêt.
         */
        void evoluer(double dt, double t_end);

        /**
         * @brief Fait évoluer le système avec l'algorithme de Störmer-Verlet (TP2).
         * @param dt Pas de temps d'itération.
         * @param t_end Temps final d'arrêt.
         */
        void evoluerVerlet(double dt, double t_end);

        /**
         * @brief Applique une vitesse scalaire uniforme à toutes les particules.
         * @param vitesse Vitesse scalaire à appliquer.
         */
        void appliquerVitesse(double vitesse);
        

        /**
         * @brief Accesseur pour la dimension de l'univers.
         * @return La dimension géométrique (1, 2, ou 3).
         */
        int getDimension() const;

        /**
         * @brief Initialise le maillage des cellules dans l'univers.
         * 
         * À appeler au début de la simulation.
         */
        void initialiserMaillage() ; // méthode pour initialiser le maillage de cellules dans l'univers, à appeler au début de la simulation

        /**
         * @brief Assigne chaque particule à la cellule correspondante.
         * 
         * À appeler après l'initialisation du maillage et après chaque déplacement.
         */
        void assignerParticulesAuxCellules() ; // méthode pour assigner chaque particule à la cellule correspondante en fonction de sa position, à appeler après l'initialisation du maillage et après chaque déplacement des particules
};

#endif