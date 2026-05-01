/**
 * @file test_classes.cpp
 * @brief Suites de tests unitaires (GTest) pour les classes du projet.
 */
#include <gtest/gtest.h>
#include "Vecteur.hpp"
#include "Particule.hpp"
#include "Cellule.hpp"
#include "Univers.hpp"

// ==========================================
// Suite de tests pour la classe Vecteur
// ==========================================

TEST(VecteurTest, ConstructeurEtGetters) {
    Vecteur v(1.5, 2.5, 3.5);
    // EXPECT_DOUBLE_EQ est recommandé pour comparer des flottants (double)
    EXPECT_DOUBLE_EQ(v.getX(), 1.5) << "La coordonnée X n'est pas initialisée correctement.";
    EXPECT_DOUBLE_EQ(v.getY(), 2.5);
    EXPECT_DOUBLE_EQ(v.getZ(), 3.5);
}

TEST(VecteurTest, Setters) {
    Vecteur v;
    v.setX(10.0);
    v.setY(-5.0);
    v.setZ(0.0);
    EXPECT_DOUBLE_EQ(v.getX(), 10.0);
    EXPECT_DOUBLE_EQ(v.getY(), -5.0);
}

TEST(VecteurTest, AdditionVecteurs) {
    Vecteur v1(1.0, 2.0, 3.0);
    Vecteur v2(4.0, 5.0, 6.0);
    Vecteur v3 = v1 + v2;
    
    EXPECT_DOUBLE_EQ(v3.getX(), 5.0);
    EXPECT_DOUBLE_EQ(v3.getY(), 7.0);
    EXPECT_DOUBLE_EQ(v3.getZ(), 9.0);
}

TEST(VecteurTest, SoustractionVecteurs) {
    Vecteur v1(5.0, 5.0, 5.0);
    Vecteur v2(2.0, 3.0, 1.0);
    Vecteur v3 = v1 - v2;
    
    EXPECT_DOUBLE_EQ(v3.getX(), 3.0);
    EXPECT_DOUBLE_EQ(v3.getY(), 2.0);
    EXPECT_DOUBLE_EQ(v3.getZ(), 4.0);
}

TEST(VecteurTest, ProduitScalaire) {
    Vecteur v1(1.0, 2.0, 3.0);
    Vecteur v2(4.0, -5.0, 6.0);
    double scalaire = v1.dot(v2); // 4 - 10 + 18 = 12
    
    EXPECT_DOUBLE_EQ(scalaire, 12.0) << "Erreur dans le calcul du produit scalaire !";
}

TEST(VecteurTest, MultiplicationParScalaire) {
    Vecteur v(2.0, -3.0, 4.0);
    Vecteur res = v * 2.5;
    
    EXPECT_DOUBLE_EQ(res.getX(), 5.0);
    EXPECT_DOUBLE_EQ(res.getY(), -7.5);
    EXPECT_DOUBLE_EQ(res.getZ(), 10.0);
}

TEST(VecteurTest, PlusEgal) {
    Vecteur v(1.0, 2.0, 3.0);
    Vecteur v2(3.0, 2.0, 1.0);
    v += v2;
    EXPECT_DOUBLE_EQ(v.getX(), 4.0);
    EXPECT_DOUBLE_EQ(v.getY(), 4.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 4.0);
}

// ==========================================
// Suite de tests pour la classe Particule
// ==========================================

TEST(ParticuleTest, Initialisation) {
    Vecteur pos(0, 0, 0);
    Vecteur vit(1, 2, 3);
    Particule p(1, "Asteroide", 10.5, pos, vit);

    EXPECT_EQ(p.getId(), 1);
    EXPECT_EQ(p.getType(), "Asteroide");
    EXPECT_DOUBLE_EQ(p.getMasse(), 10.5);
    EXPECT_DOUBLE_EQ(p.getVitesse().getY(), 2.0);
}

TEST(ParticuleTest, AccumulationForce) {
    Particule p(1, "Test", 1.0, {0,0,0}, {0,0,0});
    Vecteur f1(1.0, 0.0, 0.0);
    Vecteur f2(0.0, 2.5, 0.0);

    p.ajouterForce(f1);
    p.ajouterForce(f2);

    EXPECT_DOUBLE_EQ(p.getForce().getX(), 1.0);
    EXPECT_DOUBLE_EQ(p.getForce().getY(), 2.5);
}

TEST(ParticuleTest, UpdatePositionStomerVerlet) {
    // Masse = 2.0, Pos Initiale = (0,0,0), Vitesse = (1,0,0)
    Particule p(1, "Test", 2.0, {0, 0, 0}, {1.0, 0, 0});
    p.setForce({4.0, 0, 0}); 
    
    // pos = pos + dt * (vit + 0.5 * force / masse * dt)
    // pos_x = 0 + 0.1 * (1.0 + 0.5 * 4.0 / 2.0 * 0.1) = 0.11
    p.updatePosition(0.1);
    
    EXPECT_DOUBLE_EQ(p.getPosition().getX(), 0.11) << "L'algorithme de Störmer-Verlet (étape 1) est cassé.";
}

TEST(ParticuleTest, UpdateVitesseStomerVerlet) {
    Particule p(1, "Test", 2.0, {0, 0, 0}, {1.0, 0, 0});
    p.setForce({4.0, 0, 0}); // Nouvelle force calculée
    Vecteur ancienne_force(2.0, 0, 0);
    
    // vit = vit + dt * 0.5 / masse * (force + ancienne_force)
    // vit_x = 1.0 + 0.1 * 0.5 / 2.0 * (4.0 + 2.0) = 1.15
    p.updateVitesse(0.1, ancienne_force);
    
    EXPECT_DOUBLE_EQ(p.getVitesse().getX(), 1.15) << "L'algorithme de Störmer-Verlet (étape 3) est cassé.";
}

TEST(ParticuleTest, Setters) {
    Particule p(1, "Test", 1.0, {0,0,0}, {0,0,0});
    p.setPosition({10.0, 20.0, 30.0});
    p.setVitesse({1.0, 1.0, 1.0});
    
    EXPECT_DOUBLE_EQ(p.getPosition().getX(), 10.0);
    EXPECT_DOUBLE_EQ(p.getPosition().getY(), 20.0);
    EXPECT_DOUBLE_EQ(p.getVitesse().getZ(), 1.0);
}

// ==========================================
// Suite de tests pour la classe Cellule
// ==========================================

TEST(CelluleTest, AjoutParticuleEtVoisine) {
    Vecteur centre(0, 0, 0);
    Cellule c({}, {}, centre);
    Particule p(1, "Test", 1.0, {0,0,0}, {0,0,0});
    Cellule voisine({}, {}, {1, 0, 0});
    
    // On vérifie simplement que les méthodes ne crashent pas (pas d'exceptions levées)
    EXPECT_NO_FATAL_FAILURE(c.addParticule(&p));
    EXPECT_NO_FATAL_FAILURE(c.addVoisine(&voisine));
}

// ==========================================
// Suite de tests pour la classe Univers
// ==========================================

TEST(UniversTest, InitialisationDimensions) {
    std::deque<Particule> vide;
    Univers u(3, 0, vide, 10.0, Vecteur(100, 100, 100), {});
    
    ASSERT_EQ(u.getDimension(), 3) << "L'univers n'a pas enregistré la bonne dimension (ASSERT fatal).";
}

TEST(UniversTest, AppliquerVitesse) {
    std::deque<Particule> parts;
    // Particule avec vitesse de norme = 5 (3^2 + 4^2 = 25)
    parts.push_back(Particule(1, "P1", 1.0, {0,0,0}, {3.0, 4.0, 0.0})); 
    Univers u(3, 1, parts, 10.0, Vecteur(10,10,10), {});
    
    // On demande à appliquer une vitesse de norme 10
    u.appliquerVitesse(10.0);
    
    // Vérification avec le getter
    Vecteur nouvelle_vit = u.getParticules()[0].getVitesse();
    EXPECT_DOUBLE_EQ(nouvelle_vit.getX(), 6.0); // 3 * (10 / 5)
    EXPECT_DOUBLE_EQ(nouvelle_vit.getY(), 8.0); // 4 * (10 / 5)
    EXPECT_DOUBLE_EQ(nouvelle_vit.getZ(), 0.0);
}

TEST(UniversTest, AvancerEuler) {
    std::deque<Particule> parts;
    parts.push_back(Particule(1, "P1", 1.0, {0,0,0}, {1.0, 2.0, 3.0})); 
    Univers u(3, 1, parts, 100.0, Vecteur(10,10,10), {});
    
    // dt = 2.0. nouvelle pos = pos + vit * dt = (2.0, 4.0, 6.0)
    u.avancer(2.0);
    
    Vecteur new_pos = u.getParticules()[0].getPosition();
    EXPECT_DOUBLE_EQ(new_pos.getX(), 2.0);
    EXPECT_DOUBLE_EQ(new_pos.getY(), 4.0);
    EXPECT_DOUBLE_EQ(new_pos.getZ(), 6.0);
}

TEST(UniversTest, CalculForcesInteractions) {
    std::deque<Particule> parts;
    // Deux particules de masse 1.0, distantes de 2.0 sur l'axe X
    parts.push_back(Particule(1, "P1", 1.0, {0.0, 0.0, 0.0}, {0,0,0})); 
    parts.push_back(Particule(2, "P2", 1.0, {2.0, 0.0, 0.0}, {0,0,0})); 
    
    Univers u(3, 2, parts, 100.0, Vecteur(10,10,10), {});
    u.calculerForces();
    
    // La force totale (Gravité + Lennard-Jones) de P2 sur P1 dirigée vers +X
    // Pour dist=2.0 : Gravité = 0.25, LJ = 0.181640625 -> Total = 0.431640625
    Vecteur force_p1 = u.getParticules()[0].getForce();
    EXPECT_DOUBLE_EQ(force_p1.getX(), 0.431640625);
    EXPECT_DOUBLE_EQ(force_p1.getY(), 0.0);
}

// ==========================================
// Suite de tests pour Lennard-Jones
// ==========================================

// Déclaration locale de la fonction pour pouvoir la tester sans avoir besoin d'un .hpp dédié
double calculLennardJones(double r, double epsilon, double sigma, double rcut);

TEST(LennardJonesTest, CalculsClassiques) {
    // Au-delà de rcut, le potentiel est nul
    EXPECT_DOUBLE_EQ(calculLennardJones(5.0, 1.0, 1.0, 2.5), 0.0);
    
    // Si r == sigma, le potentiel est 0
    EXPECT_DOUBLE_EQ(calculLennardJones(1.0, 1.0, 1.0, 5.0), 0.0);
    
}