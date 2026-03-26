cmake_minimum_required(VERSION 3.10)
project(TP_PARTICULE)

# On force le standard C++17 (très commun en maths appliquées)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE MES_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(mon_programme ${MES_SOURCES})

target_include_directories(mon_programme PRIVATE include)