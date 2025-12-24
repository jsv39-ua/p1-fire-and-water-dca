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
