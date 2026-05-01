#ifndef PARTICULE_HPP
#define PARTICULE_HPP

#include <string>
#include <array>
#include "Vecteur.hpp"

using Vec2 = std::array<double, 2>;

/**
 * @brief Représente une particule physique dans l'univers.
 * 
 * Gère les propriétés d'une particule (masse, position, vitesse, force).
 */
class Particule{
    private:
        int id;
        std::string type;
        double masse;
        Vecteur position;
        Vecteur vitesse;
        Vecteur force;

    public:
        /**
         * @brief Affiche les informations de la particule.
         */
        void afficher() const;

        /**
         * @brief Constructeur de la Particule.
         * @param id Identifiant unique de la particule.
         * @param type Type de la particule (ex: "Soleil").
         * @param masse Masse de la particule.
         * @param pos Vecteur position initiale.
         * @param vitesse Vecteur vitesse initiale.
         */
        Particule(int id, std::string type, double masse, Vecteur pos, Vecteur vitesse);

        // Getters
        /**
         * @brief Récupère l'identifiant de la particule.
         * @return L'identifiant entier.
         */
        int getId() const;
        /**
         * @brief Récupère le type de la particule.
         * @return Une chaîne de caractères représentant le type.
         */
        std::string getType() const;
        /**
         * @brief Récupère la masse de la particule.
         * @return La masse.
         */
        double getMasse() const;
        /**
         * @brief Récupère la position de la particule.
         * @return Le vecteur position.
         */
        Vecteur getPosition() const;
        /**
         * @brief Récupère la vitesse de la particule.
         * @return Le vecteur vitesse.
         */
        Vecteur getVitesse() const;
        /**
         * @brief Récupère la force appliquée à la particule.
         * @return Le vecteur force.
         */
        Vecteur getForce() const; 
        
        // Setters
        /** @brief Modifie la position. */
        void setPosition(const Vecteur& pos);
        /** @brief Modifie la vitesse. */
        void setVitesse(const Vecteur& vitesse);
        /** @brief Modifie la force. */
        void setForce(const Vecteur& f);

        /**
         * @brief Ajoute une force au vecteur force actuel.
         * @param f Vecteur force à ajouter.
         */
        void ajouterForce(Vecteur& f);
};

#endif