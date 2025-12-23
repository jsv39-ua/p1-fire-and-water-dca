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

# raytmx/hoxml (vendor): MSVC en C++ no soporta algunos constructs C99 usados por raytmx.h.
# Para no modificar la librería en el repo, generamos una copia parcheada en el build dir
# y la ponemos antes en el include path (solo MSVC).
set(RAYTMX_GENERATED_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated")
set(RAYTMX_VENDOR_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/vendor/include")

if(MSVC)
  file(MAKE_DIRECTORY "${RAYTMX_GENERATED_INCLUDE_DIR}")
  set(RAYTMX_IN "${RAYTMX_VENDOR_INCLUDE_DIR}/raytmx.h")
  set(RAYTMX_OUT "${RAYTMX_GENERATED_INCLUDE_DIR}/raytmx.h")
  if(EXISTS "${RAYTMX_IN}")
    file(READ "${RAYTMX_IN}" RAYTMX_CONTENTS)

    # 1) C99 compound literal: (Rectangle){0.0f}  ->  Rectangle{0.0f}
    string(REPLACE "(Rectangle){0.0f}" "Rectangle{0.0f}" RAYTMX_CONTENTS "${RAYTMX_CONTENTS}")

    # 2) C99 designated initializer for Rectangle -> C++ aggregate init
    string(REGEX REPLACE "\\*tileRect = \\(Rectangle\\) \\{[\\r\\n[:space:]]*\\.x = ([^,]+),[\\r\\n[:space:]]*\\.y = ([^,]+),[\\r\\n[:space:]]*\\.width = ([^,]+),[\\r\\n[:space:]]*\\.height = ([^\\r\\n;]+)[\\r\\n[:space:]]*\\};" "*tileRect = Rectangle{\\1,\\2,\\3,\\4};" RAYTMX_CONTENTS "${RAYTMX_CONTENTS}")

    file(WRITE "${RAYTMX_OUT}" "${RAYTMX_CONTENTS}")
    message(STATUS "MSVC: using generated raytmx.h at ${RAYTMX_OUT}")
  endif()
endif()

target_include_directories(game_core
  PUBLIC
    ${CMAKE_SOURCE_DIR}/core/include
    ${RAYTMX_GENERATED_INCLUDE_DIR}
    ${RAYTMX_VENDOR_INCLUDE_DIR}
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