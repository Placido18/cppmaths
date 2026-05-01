cmake_minimum_required(VERSION 3.10)
project(ProjetCpp_Neuilly_Meniane)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# on crée une bibliothèque
add_library(librairie
    src/Particule.cpp
    src/Vecteur.cpp
    src/Univers.cpp)

target_include_directories(librairie PUBLIC include)

# Ajout des sous-répertoires pour la compilation modulaire
add_subdirectory(demo)
add_subdirectory(test)
add_subdirectory(doc)