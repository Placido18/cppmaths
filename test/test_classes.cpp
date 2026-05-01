#include <gtest/gtest.h>
#include "Vecteur.hpp"
#include "Particule.hpp"

// ==========================================
// Suite de tests pour la classe Vecteur
// ==========================================

TEST(VecteurTest, ConstructeurEtGetters) {
    Vecteur v(1.5, 2.5, 3.5);
    EXPECT_EQ(v.getX(), 1.5);
    EXPECT_EQ(v.getY(), 2.5);
    EXPECT_EQ(v.getZ(), 3.5);
}

TEST(VecteurTest, AdditionVecteurs) {
    Vecteur v1(1.0, 2.0, 3.0);
    Vecteur v2(4.0, 5.0, 6.0);
    Vecteur v3 = v1 + v2;
    
    EXPECT_EQ(v3.getX(), 5.0);
    EXPECT_EQ(v3.getY(), 7.0);
    EXPECT_EQ(v3.getZ(), 9.0);
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
    EXPECT_EQ(p.getMasse(), 10.5);
    EXPECT_EQ(p.getVitesse().getY(), 2.0);
}

TEST(ParticuleTest, AccumulationForce) {
    Particule p(1, "Test", 1.0, {0,0,0}, {0,0,0});
    Vecteur f1(1.0, 0.0, 0.0);
    Vecteur f2(0.0, 2.5, 0.0);

    p.ajouterForce(f1);
    p.ajouterForce(f2);

    EXPECT_EQ(p.getForce().getX(), 1.0);
    EXPECT_EQ(p.getForce().getY(), 2.5);
}