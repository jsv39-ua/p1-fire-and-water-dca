add_executable(game.bin
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(game.bin PRIVATE game_core)

install(TARGETS game.bin
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/p1-fire-and-water-dca
)