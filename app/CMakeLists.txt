add_executable(game.bin
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(game.bin PRIVATE game_core)

# Directorio de datos del juego
set(GAME_DATA_DIR ${CMAKE_INSTALL_DATAROOTDIR}/p1-fire-and-water-dca)

if(WIN32)
  # Windows: instalación portable (exe + assets en la misma carpeta)
  install(TARGETS game.bin
    RUNTIME DESTINATION .
  )
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
    DESTINATION .
  )
else()
  # Linux: binario en /usr/share/... con wrapper en /usr/bin
  install(TARGETS game.bin
    RUNTIME DESTINATION ${GAME_DATA_DIR}
  )
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
    DESTINATION ${GAME_DATA_DIR}
  )
  
  # Wrapper script para lanzar desde terminal
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/p1-fire-and-water-dca
"#!/bin/sh
cd /usr/share/p1-fire-and-water-dca && exec ./game.bin \"$@\"
")
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/p1-fire-and-water-dca
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()