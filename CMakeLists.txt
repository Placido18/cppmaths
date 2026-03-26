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


# Programme principal
add_executable(main src/main.cpp)

# on lie la bibliothèque au programme principal
target_link_libraries(main librairie)


# programmes de test, pour les questions des tps
add_executable(tp3 test/tp3.cpp)
target_link_libraries(tp3 librairie)