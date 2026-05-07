#ifndef UNIVERS_HPP
#define UNIVERS_HPP
#include <deque>
#include "Particule.hpp"
#include "Vecteur.hpp"
#include "Cellule.hpp"

#include <vector>

/**
 * @brief Types de conditions aux limites applicables aux parois du domaine.
 */
enum class ConditionLimite {
    REFLEXION,           ///< Rebond élastique : les composantes de vitesse perpendiculaires changent de signe.
    ABSORPTION,          ///< Les particules disparaissent lorsqu'elles touchent la paroi.
    PERIODIQUE,          ///< La particule est transportée sur la paroi opposée sans modification de trajectoire.
    REFLEXION_POTENTIEL  ///< Répulsion par potentiel de Lennard-Jones modifié (Q3).
};

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

        double epsilon;
        double sigma;
        bool use_gravity;
        bool use_LJ;

        // Conditions aux limites par direction
        ConditionLimite cond_x;
        ConditionLimite cond_y;
        ConditionLimite cond_z;

        // Champ gravitationnel uniforme externe (distinct de la gravité inter-particules)
        bool use_champ_gravite;
        double G_champ;

        // Énergie cinétique cible pour le rescaling (0 = désactivé)
        double Ec_cible;

    public:
        /**
         * @brief Constructeur de l'Univers.
         * @param dimension Dimension de l'espace (1, 2 ou 3).
         * @param nb_particules Nombre de particules dans l'univers.
         * @param particules Conteneur contenant toutes les particules.
         * @param rcut Rayon de coupure pour les interactions et taille des cellules.
         * @param Ld Dimensions géométriques de l'univers.
         * @param cellules Liste des cellules formant le maillage.
         * @throws std::invalid_argument si dimension n'est pas dans {1,2,3} ou si rcut ≤ 0.
         */
        Univers(int dimension, int nb_particules, std::deque<Particule> particules, double rcut, Vecteur Ld, std::vector<Cellule> cellules);

        /**
         * @brief Fait avancer les particules pendant un pas de temps.
         * @param dt Pas de temps de la simulation.
         * @throws std::invalid_argument si dt ≤ 0.
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
         * @throws std::invalid_argument si dt ≤ 0 ou t_end ≤ 0.
         */
        void evoluerVerlet(double dt, double t_end, int vtk_freq = 100);

        /**
         * @brief Configure les paramètres physiques de la simulation.
         * @param eps Profondeur du potentiel Lennard-Jones.
         * @param sig Distance d'interaction Lennard-Jones.
         * @param gravity Active la force gravitationnelle.
         * @param lj Active le potentiel de Lennard-Jones.
         * @throws std::invalid_argument si eps ≤ 0 ou sig ≤ 0.
         */
        void setPhysicsParams(double eps, double sig, bool gravity = false, bool lj = true);

        /**
         * @brief Applique une vitesse scalaire uniforme à toutes les particules.
         * @param vitesse Vitesse scalaire à appliquer.
         */
        void appliquerVitesse(double vitesse);

        /**
         * @brief Configure les conditions aux limites par direction.
         * @param cx Condition pour les parois en x.
         * @param cy Condition pour les parois en y.
         * @param cz Condition pour les parois en z.
         */
        void setConditionsLimites(ConditionLimite cx, ConditionLimite cy, ConditionLimite cz);

        /**
         * @brief Active le champ gravitationnel uniforme externe.
         * @param G Intensité du champ (négatif = vers le bas).
         */
        void setChampGravite(double G);

        /**
         * @brief Active le rescaling de vitesse vers une énergie cinétique cible.
         * @param Ec Énergie cinétique cible (0 = désactivé).
         */
        void setEnergieCinetiqueCible(double Ec);

        /**
         * @brief Accesseur pour la dimension de l'univers.
         * @return La dimension géométrique (1, 2, ou 3).
         */
        int getDimension() const;

        /**
         * @brief Accesseur pour récupérer la liste des particules.
         * @return Une référence constante vers la deque de particules.
         */
        const std::deque<Particule>& getParticules() const;

        /**
         * @brief Sauvegarde l'état actuel de l'univers au format VTK (XML).
         *
         * @param iteration Le numéro de l'itération actuelle (pour nommer le fichier).
         */
        void sauvegarderVTK(int iteration) const;

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
