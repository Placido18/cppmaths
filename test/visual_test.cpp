/**
 * @file visual_test.cpp
 * @brief Test de la visualisation VTK (TP5 - Question 3).
 *
 * Lance une courte simulation et vérifie que les fichiers VTK sont générés
 * correctement au format UnstructuredGrid lisible par ParaView.
 */

#include "Univers.hpp"
#include <iostream>
#include <fstream>
#include <deque>
#include <cmath>
#include <sys/stat.h>

static bool fileExists(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

static bool vtkIsValid(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::string line;
    std::getline(f, line);
    return line.find("VTKFile") != std::string::npos;
}

// ── Test 1 : simulation gravitationnelle 2 corps ──────────────────────
bool testGravite() {
    std::deque<Particule> parts;
    parts.push_back(Particule(1, "Soleil", 1.0,   {0.0, 0.0, 0.0}, {0.0,  0.0, 0.0}));
    parts.push_back(Particule(2, "Terre",  3.0e-6, {1.0, 0.0, 0.0}, {0.0,  1.0, 0.0}));

    Univers u(3, 2, parts, 1000.0, Vecteur(100, 100, 100), {});
    u.setPhysicsParams(1.0, 1.0, /*gravity=*/true, /*lj=*/false);

    // vtk_freq=5 → 2 fichiers pour 10 itérations (output_0000 et output_0001)
    u.evoluerVerlet(0.01, 0.1, 5);

    bool ok = fileExists("output_0000.vtu") &&
              fileExists("output_0001.vtu") &&
              vtkIsValid("output_0000.vtu");
    std::cout << "[Test 1 - Gravité VTK]   " << (ok ? "PASS" : "FAIL") << "\n";
    return ok;
}

// ── Test 2 : simulation LJ 2 corps ───────────────────────────────────
bool testLennardJones() {
    std::deque<Particule> parts;
    // Distance d'équilibre ≈ 1.1225 (minimum du puits LJ)
    double d = std::pow(2.0, 1.0 / 6.0);
    parts.push_back(Particule(1, "rouge", 1.0, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}));
    parts.push_back(Particule(2, "bleu",  1.0, {d,   0.0, 0.0}, {0.0, 0.0, 0.0}));

    Univers u(3, 2, parts, 2.5, Vecteur(10, 10, 10), {});
    u.setPhysicsParams(5.0, 1.0, /*gravity=*/false, /*lj=*/true);

    u.evoluerVerlet(0.0001, 0.001, 5);

    bool ok = fileExists("output_0000.vtu") && vtkIsValid("output_0000.vtu");
    std::cout << "[Test 2 - LJ VTK]        " << (ok ? "PASS" : "FAIL") << "\n";
    return ok;
}

// ── Test 3 : champ Groupe dans le VTK ────────────────────────────────
bool testGroupeVTK() {
    std::deque<Particule> parts;
    parts.push_back(Particule(1, "rouge", 1.0, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}));
    parts.push_back(Particule(2, "bleu",  1.0, {5.0, 0.0, 0.0}, {0.0, 0.0, 0.0}));

    Univers u(3, 2, parts, 100.0, Vecteur(20, 20, 20), {});
    u.setPhysicsParams(1.0, 1.0, false, false);
    u.evoluerVerlet(0.01, 0.01, 1);

    std::ifstream f("output_0000.vtu");
    std::string contenu((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    bool ok = contenu.find("Groupe") != std::string::npos;
    std::cout << "[Test 3 - Champ Groupe]  " << (ok ? "PASS" : "FAIL") << "\n";
    return ok;
}

int main() {
    std::cout << "=== Tests de visualisation VTK (TP5 Q3) ===\n";

    bool all_ok = true;
    all_ok &= testGravite();
    all_ok &= testLennardJones();
    all_ok &= testGroupeVTK();

    std::cout << (all_ok ? "\nTous les tests passent.\n" : "\nEchec(s) detecte(s).\n");
    return all_ok ? 0 : 1;
}
