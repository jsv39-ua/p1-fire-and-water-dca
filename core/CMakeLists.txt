set(GAME_CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/components/Animation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/components/Camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Enemy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Entity.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Platform.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Player.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/entities/Projectile.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/GameOverState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/GameState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/KeyBindingState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/MainGameState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/MainMenuState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/OptionsState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/PauseMenuState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/SimpleLevelState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/StateMachine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/AudioManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/LocalizationManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/MapLoader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/raytmx_impl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/ResourceManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/SettingsManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/systems/SoundPlayer.cpp
)

# Creamos la librería estática 'game_core' con los fuentes del núcleo del juego
add_library(game_core STATIC ${GAME_CORE_SOURCES})

target_include_directories(game_core
  PUBLIC
    ${CMAKE_SOURCE_DIR}/core/include
    ${CMAKE_SOURCE_DIR}/vendor/include
)

# Enlazamos Raylib al núcleo del juego
target_link_libraries(game_core
  PUBLIC
    raylib
)

# Gettext/Intl: solo si está disponible (o si el usuario lo fuerza y existe)
if(DEFINED HAVE_GETTEXT AND HAVE_GETTEXT)
  target_compile_definitions(game_core PUBLIC HAVE_GETTEXT=1)
  if(TARGET Intl::Intl)
    target_link_libraries(game_core PUBLIC Intl::Intl)
  endif()
else()
  target_compile_definitions(game_core PUBLIC HAVE_GETTEXT=0)
endif()