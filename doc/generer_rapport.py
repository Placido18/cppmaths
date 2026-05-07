#!/usr/bin/env python3
"""Génère le rapport Lab4 au format ODT avec odfpy."""

from odf.opendocument import OpenDocumentText
from odf.style import Style, TextProperties, ParagraphProperties
from odf.text import P, H, Span, LineBreak
import odf.text as odftext

doc = OpenDocumentText()

# ── Styles ──────────────────────────────────────────────────────────────
def make_style(doc, name, family, parent=None, para_props=None, text_props=None):
    s = Style(name=name, family=family)
    if parent:
        s.setAttribute("parentstylename", parent)
    if para_props:
        s.addElement(ParagraphProperties(**para_props))
    if text_props:
        s.addElement(TextProperties(**text_props))
    doc.styles.addElement(s)
    return s

make_style(doc, "Titre1", "paragraph", parent="Heading 1")

def h1(text):
    p = H(outlinelevel=1)
    p.addText(text)
    doc.text.addElement(p)

def h2(text):
    p = H(outlinelevel=2)
    p.addText(text)
    doc.text.addElement(p)

def para(text):
    p = P()
    p.addText(text)
    doc.text.addElement(p)

def blank():
    doc.text.addElement(P())

# ── Contenu ─────────────────────────────────────────────────────────────
title = H(outlinelevel=1)
title.addText("Lab 4 – Découpage de l'espace & Collision Lennard-Jones")
doc.text.addElement(title)
blank()

para("Auteurs : Neuilly, Meniane  |  Cours : C++ pour les mathématiques appliquées – Grenoble INP")
blank()

# --- 1. Introduction ---
h1("1. Objectif")
para(
    "L'objectif de ce Lab est double : (1) introduire le potentiel de Lennard-Jones (LJ) "
    "comme modèle d'interaction entre particules et (2) réduire la complexité du calcul "
    "des forces de O(N²) à O(N) grâce au découpage de l'espace en cellules."
)
blank()

# --- 2. Potentiel LJ ---
h1("2. Potentiel de Lennard-Jones")
para(
    "Le potentiel entre deux particules séparées d'une distance r est :"
)
para("    U(r) = 4ε [ (σ/r)¹² − (σ/r)⁶ ]   si r ≤ rcut,  0 sinon.")
blank()
para(
    "La force élémentaire correspondante (dérivée du potentiel global, antisymétrique) vaut :"
)
para("    F_ij = 24ε/r² · (σ/r)⁶ · (1 − 2(σ/r)⁶) · r_ij")
blank()
para(
    "Le minimum de potentiel se situe en r = 2^(1/6)·σ ≈ 1.1225·σ, "
    "distance utilisée comme espacement initial entre particules à l'équilibre."
)
blank()

# --- 3. Classe Univers ---
h1("3. Classe Univers et paramètres physiques")
para(
    "La classe Univers a été enrichie des champs epsilon, sigma, use_gravity et use_LJ "
    "(initialisés par setPhysicsParams). Cela permet de basculer entre simulation "
    "gravitationnelle (use_gravity=true, use_LJ=false) et simulation LJ (use_gravity=false, "
    "use_LJ=true) sans dupliquer le code."
)
blank()

# --- 4. Maillage et cellules ---
h1("4. Maillage en cellules et algorithme des voisines")
para(
    "L'espace est découpé en une grille tensorielle de cubes de côté rcut. "
    "Le nombre de cellules dans chaque direction d est :"
)
para("    ncd_d = floor(Ld / rcut)")
blank()
para(
    "Chaque cellule stocke des pointeurs vers les particules qu'elle contient "
    "et une liste de cellules voisines (jusqu'à 8 en 2D, 26 en 3D)."
)
blank()
para(
    "Algorithme de calculerForces() (demi-coquille, Q4) :"
)
para("  Pour chaque cellule C :")
para("    Pour chaque paire (i, j) dans C (i < j) : calculer F_ij.")
para("    Pour chaque cellule voisine V de C telle que adresse(V) > adresse(C) :")
para("      Pour chaque particule i de C, j de V :")
para("        Si ||r_ij|| ≤ rcut : calculer F_ij et appliquer −F_ij à j.")
blank()
para(
    "La condition adresse(V) > adresse(C) évite le double comptage des paires "
    "inter-cellules tout en restant correct. Complexité : O(N × 27) au lieu de O(N²)."
)
blank()
para(
    "Après chaque pas de temps, avancer() met à jour l'appartenance cellulaire "
    "de chaque particule qui a changé de cellule (Q5)."
)
blank()

# --- 5. Intégrateur Störmer-Verlet ---
h1("5. Intégrateur de Störmer-Verlet")
para(
    "L'algorithme utilisé dans evoluerVerlet() est :"
)
para("  1. x(t+dt) = x(t) + dt·v(t) + dt²/(2m)·F(t)   [via avancer()]")
para("  2. F(t+dt) calculé avec les nouvelles positions   [calculerForces()]")
para("  3. v(t+dt) = v(t) + dt/(2m)·(F(t) + F(t+dt))  [updateVitesse()]")
blank()
para(
    "C'est un intégrateur symplectique d'ordre 2, qui conserve l'énergie à long terme."
)
blank()

# --- 6. Simulation de collision ---
h1("6. Simulation de collision (Question 6)")

h2("6.1 Paramètres")
para("  ε = 5,  σ = 1,  rcut = 2.5σ = 2.5,  δt = 0.00005,  t_end = 19.5")
para("  Espacement inter-particulaire : d = 2^(1/6)·σ ≈ 1.1225")
blank()
para("  Rectangle bleu (N2 = 6400 = 160×40 particules) :")
para("    – Positionné en bas de l'univers, centré horizontalement.")
para("    – Vitesse initiale : v = (0, 0).")
blank()
para("  Carré rouge (N1 = 1600 = 40×40 particules) :")
para("    – Positionné au-dessus du rectangle avec un écart de 10 unités.")
para("    – Vitesse initiale : v = (0, −10)  [tombe vers le rectangle].")
blank()

h2("6.2 Univers")
para(
    "Univers 2D de dimensions Lx=250 × Ly=120. "
    "Le maillage comporte ncd_x = 100 et ncd_y = 48 cellules. "
    "Seul le potentiel LJ est activé (use_gravity=false)."
)
blank()

h2("6.3 Initialisation des particules")
para(
    "Les particules sont disposées sur une grille régulière d'espacement d. "
    "Le rectangle bleu occupe les positions (rect_x0 + ix·d, rect_y0 + iy·d) "
    "pour ix ∈ [0,159] et iy ∈ [0,39]. Le carré rouge est centré sur la même "
    "abscisse et placé juste au-dessus."
)
blank()

h2("6.4 Sorties")
para(
    "Les fichiers VTK (*.vtu) sont générés toutes les 1000 itérations "
    "(≈390 fichiers pour t_end=19.5). Chaque fichier contient :"
)
para("  – Positions des particules.")
para("  – Vitesses.")
para("  – Masses.")
para("  – Groupe (0 = bleu, 1 = rouge) : permet de colorier les deux objets dans ParaView.")
blank()

h2("6.5 Résultats attendus")
para(
    "Au temps t=0 : deux blocs bien séparés (carré rouge au-dessus, "
    "rectangle bleu en bas). À t=19.5 : le carré a percuté le rectangle, "
    "les deux objets se déforment et des particules sont éjectées, "
    "reproduisant une collision quasi-élastique typique des simulations LJ."
)
blank()

# --- 7. Vérification ---
h1("7. Stratégie de vérification")
para(
    "Pour valider l'implémentation LJ et le maillage, on peut :"
)
para("  a) Conserver la force gravitationnelle et vérifier que les orbites keplériennes "
     "sont correctes (régression par rapport au TP3).")
para("  b) Placer deux particules isolées et vérifier analytiquement que la force calculée "
     "correspond à la formule F_ij = 24ε/r² · (σ/r)⁶ · (1−2(σ/r)⁶) · r_ij.")
para("  c) Vérifier la conservation de l'énergie totale (cinétique + potentielle LJ) "
     "sur un système périodique stable.")
blank()

# ── TP5 : Tests et Visualisation ──────────────────────────────────────
h1("TP5 – Tests et Visualisation")
blank()

h2("Q1 & Q2 – Infrastructure de tests (GTest)")
para(
    "L'infrastructure GTest est intégrée via CMake (FetchContent, v1.17.0). "
    "Le fichier test/test_classes.cpp contient 26 tests unitaires organisés en 5 suites :"
)
para("  • VecteurTest (7 tests) : constructeurs, getters/setters, opérations vectorielles, "
     "produit scalaire, vecteur nul.")
para("  • ParticuleTest (5 tests) : initialisation, accumulation de force, Störmer-Verlet "
     "position et vitesse, setters.")
para("  • CelluleTest (3 tests) : ajout, suppression de particule, liste des voisines.")
para("  • UniversTest (6 tests) : dimensions, vitesse, avancer, calcul de forces "
     "(Gravité+LJ), action-réaction (Newton 3), absence de force au-delà de rcut, maillage 2D.")
para("  • LennardJonesTest (3 tests) : potentiel nul au-delà de rcut, U(σ)=0, minimum "
     "U(r*)=-ε à r*=2^(1/6)σ, zone répulsive.")
blank()
para("Résultat : 26/26 tests passent (ctest --output-on-failure).")
blank()

h2("Q3 – Export VTK")
para(
    "La méthode Univers::sauvegarderVTK(int iteration) génère des fichiers "
    "output_XXXX.vtu au format UnstructuredGrid XML lisibles par ParaView. "
    "Chaque fichier contient : Position (3D), Velocity (3D), Masse (scalaire), "
    "Groupe (entier : 0=bleu, 1=rouge pour colorier les objets)."
)
blank()
para(
    "Un programme de test dédié (test/visual_test.cpp) vérifie automatiquement "
    "que les fichiers sont créés et que leur entête XML est valide."
)
blank()

h1("ACVL – Analyse Conceptuelle et Visuelle du Logiciel")
blank()

h2("Q4 – Diagramme des cas d'utilisation")
para("Acteur : Physicien / Ingénieur")
blank()
para("Cas d'utilisation principaux :")
para("  UC1 – Configurer les paramètres (ε, σ, rcut, dt, t_end).")
para("  UC2 – Créer les particules (position initiale, vitesse, masse).")
para("  UC3 – Lancer la simulation Störmer-Verlet.")
para("        <<include>> UC4 : Calculer les forces (LJ / Gravité).")
para("        <<include>> UC5 : Mettre à jour les cellules spatiales.")
para("        <<include>> UC6 : Exporter les données VTK.")
para("  UC7 – Visualiser les résultats dans ParaView. <<extend>> UC6.")
blank()
para("(Voir doc/diagrammes.puml @startuml cas_utilisation pour la version PlantUML.)")
blank()

h2("Q5 – Diagramme de séquence (evoluerVerlet)")
para("Voir doc/diagrammes.puml @startuml sequence.")
blank()
para("Séquence principale :")
para("  1. Utilisateur → Univers : evoluerVerlet(dt, t_end)")
para("  2. Univers → calculerForces() : initialise les forces")
para("  3. Univers → sauvegarderVTK(0)")
para("  loop [t < t_end] :")
para("    4. Univers → avancer(dt) :")
para("         Particule::updatePosition(dt)")
para("         Cellule::removeParticule / addParticule [si changement]")
para("    5. Univers → calculerForces() [avec nouvelles positions]")
para("    6. Univers → Particule::updateVitesse(dt, f_old)")
para("    7. Univers → sauvegarderVTK() [si iteration % vtk_freq == 0]")
blank()

h2("Q6 – Diagramme de transitions")
para("Voir doc/diagrammes.puml @startuml transitions.")
blank()
para("États :")
para("  [*] → Initialisation → MaillageCreé → ParticulesAssignées")
para("  ParticulesAssignées → ForcesCalculées")
para("  loop :")
para("    ForcesCalculées → PositionsMàJ → CellulesàJour")
para("    → NouvellesForcesCalculées → VitessesàJour")
para("    → VTKSauvegardé [si freq] → PositionsMàJ [si t < t_end]")
para("  ForcesCalculées / VTKSauvegardé → Terminé [si t >= t_end] → [*]")
blank()

h2("Q7 – Diagramme de classes d'analyse")
para("Voir doc/diagrammes.puml @startuml classes.")
blank()
para("Classes et relations :")
para("  Vecteur : x, y, z + opérations vectorielles.")
para("  Particule : id, type, masse + position/vitesse/force (Vecteur) + Störmer-Verlet.")
para("  Cellule : liste de Particule* (référence, sans possession) + liste de Cellule* voisines.")
para("  Univers : deque<Particule> (possession) + vector<Cellule> (possession) "
     "+ paramètres physiques (ε, σ, use_gravity, use_LJ).")
blank()
para("Associations :")
para("  Univers 1 *-- N Particule   (composition)")
para("  Univers 1 *-- M Cellule     (composition)")
para("  Cellule  1 o-- 0..* Particule (agrégation par pointeur)")
para("  Cellule  1 o-- 0..26 Cellule  (agrégation : voisines)")
para("  Particule 1 *-- 3 Vecteur    (composition : position, vitesse, force)")
blank()

# ── Sauvegarde ──────────────────────────────────────────────────────────
out = "/home/menianer/ensimag/C++curse/cppmaths/doc/rapport_lab4.odt"
doc.save(out)
print(f"ODT généré : {out}")
