find_program(MSGFMT_EXECUTABLE msgfmt)

if(MSGFMT_EXECUTABLE)
    message(STATUS "msgfmt encontrado: ${MSGFMT_EXECUTABLE}")
    
    # Buscar todos los archivos .po en esta carpeta y subcarpetas
    file(GLOB_RECURSE PO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.po")

    set(MO_FILES)
    foreach(PO_FILE ${PO_FILES})
        # Cambiar po por mo para el nombre del archivo compilado
        string(REPLACE ".po" ".mo" MO_FILE ${PO_FILE})
        list(APPEND MO_FILES ${MO_FILE})

        # Compilacion
        add_custom_command(
            OUTPUT ${MO_FILE}
            COMMAND ${MSGFMT_EXECUTABLE} -o ${MO_FILE} ${PO_FILE}
            DEPENDS ${PO_FILE}
            COMMENT "Compilando traducción: ${PO_FILE} -> ${MO_FILE}"
        )
    endforeach()

    # target para compilar todas las traducciones
    add_custom_target(compile_translations ALL DEPENDS ${MO_FILES})

else()
    message(WARNING "msgfmt (gettext) no encontrado.")
endif()

# target para actualizar los archivos de traduccion al añadir mas texto en el codigo
find_program(XGETTEXT_EXECUTABLE xgettext)
find_program(MSGMERGE_EXECUTABLE msgmerge)

if(XGETTEXT_EXECUTABLE AND MSGMERGE_EXECUTABLE)
    # Buscar archivos fuente para traducción
    file(GLOB_RECURSE TRANSLATION_SOURCES 
        "${CMAKE_SOURCE_DIR}/core/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/app/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/core/include/*.hpp"
    )

    add_custom_target(update_translations
        # Generar template.pot buscando en src e include
        COMMAND ${XGETTEXT_EXECUTABLE} --keyword=_ --language=C++ --add-comments --sort-output --no-location --from-code=UTF-8 -o assets/locales/template.pot ${TRANSLATION_SOURCES}
        # Actualizar PO Español
        COMMAND ${MSGMERGE_EXECUTABLE} --update --no-fuzzy-matching --backup=off assets/locales/es/LC_MESSAGES/fire_water_game.po assets/locales/template.pot
        # Actualizar PO Inglés
        COMMAND ${MSGMERGE_EXECUTABLE} --update --no-fuzzy-matching --backup=off assets/locales/en/LC_MESSAGES/fire_water_game.po assets/locales/template.pot
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Actualizando template.pot y fusionando cambios en archivos .po"
        VERBATIM
    )
endif()
