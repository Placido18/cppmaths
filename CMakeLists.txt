cmake_minimum_required(VERSION 3.20)
project(ProjetCpp_Neuilly_Meniane)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Configuration de Google Test ---
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)

if(APPLE)
  foreach(_gtest_target IN ITEMS gtest gtest_main gmock gmock_main)
    if(TARGET ${_gtest_target})
      set_property(TARGET ${_gtest_target} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "")
    endif()
  endforeach()
endif()

enable_testing()
# ------------------------------------

file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")
add_library(librairie ${SRC_FILES})

target_include_directories(librairie PUBLIC include)

# Ajout des sous-répertoires pour la compilation modulaire
add_subdirectory(demo)
add_subdirectory(test)
add_subdirectory(doc)