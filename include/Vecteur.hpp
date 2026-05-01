#ifndef VECTEUR_HPP
#define VECTEUR_HPP

/**
 * @brief Représente un vecteur mathématique en 3 dimensions (x, y, z).
 * 
 * Utilisé pour représenter des positions, des vitesses et des forces dans l'espace.
 */
class Vecteur {
    private:
        double x;
        double y;
        double z;

    public:
        /** @brief Constructeur par défaut (vecteur nul). */
        Vecteur();
        /**
         * @brief Constructeur avec coordonnées.
         * @param x Composante X.
         * @param y Composante Y.
         * @param z Composante Z.
         */
        Vecteur(double x, double y, double z);

        // Getters
        /** @brief Récupère la composante X. */
        double getX() const;
        /** @brief Récupère la composante Y. */
        double getY() const;
        /** @brief Récupère la composante Z. */
        double getZ() const;

        // Setters
        /** @brief Modifie la composante X. */
        void setX(double x);
        /** @brief Modifie la composante Y. */
        void setY(double y);
        /** @brief Modifie la composante Z. */
        void setZ(double z);

        // Opérations vectorielles
        /**
         * @brief Additionne deux vecteurs.
         * @param v Vecteur à ajouter.
         * @return Nouveau vecteur résultat de l'addition.
         */
        Vecteur operator+(const Vecteur& v) const;
        /**
         * @brief Soustrait un vecteur.
         * @param v Vecteur à soustraire.
         * @return Nouveau vecteur résultat de la soustraction.
         */
        Vecteur operator-(const Vecteur& v) const;
        /**
         * @brief Multiplie le vecteur par un scalaire.
         * @param scalar Valeur du scalaire.
         * @return Nouveau vecteur multiplié.
         */
        Vecteur operator*(double scalar) const;
        /**
         * @brief Calcule le produit scalaire avec un autre vecteur.
         * @param v L'autre vecteur.
         * @return Le résultat du produit scalaire.
         */
        double dot(const Vecteur& v) const; // Produit scalaire
        /**
         * @brief Ajoute un vecteur à l'instance courante.
         * @param v Vecteur à ajouter.
         * @return Référence vers l'instance modifiée.
         */
        Vecteur operator+=(const Vecteur& v);
};

#endif 