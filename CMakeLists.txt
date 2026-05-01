cmake_minimum_required(VERSION 3.10)
project(ProjetCpp_Neuilly_Meniane)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
add_library(librairie ${SRC_FILES})

target_include_directories(librairie PUBLIC include)

# Ajout des sous-répertoires pour la compilation modulaire
add_subdirectory(demo)
add_subdirectory(test)
add_subdirectory(doc)