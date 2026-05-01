find_package(Doxygen)
if (DOXYGEN_FOUND)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/doxyfilerc
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Génération de la documentation Doxygen..."
        VERBATIM)
else()
    message(WARNING "Doxygen n'est pas installé. Impossible de générer la documentation.")
endif()