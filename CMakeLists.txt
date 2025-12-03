# CMake configuration for p1-fire-and-water-dca
#
# Design goals:
# - Modern CMake using targets
# - DRY: file(GLOB_RECURSE) for sources, central includes
# - FetchContent to download Raylib automatically
# - CPack module in cmake/CPackConfig.cmake for packaging (deb, zip)
# - Separated responsibilities: static library "game_core" and executable "game"
#
cmake_minimum_required(VERSION 3.21)
project(p1-fire-and-water-dca
    VERSION 0.1.0
    DESCRIPTION "Fire & Water - DCA Project"
    LANGUAGES CXX C)

# Enforce C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------------------------------------------
# Options
# ----------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build libraries as shared" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

# ----------------------------------------------------------------------------
# Dependencies (FetchContent)
# ----------------------------------------------------------------------------
include(FetchContent)
# Raylib: We'll fetch and build a specific release to guarantee reproducibility.
# Force a reasonably recent tag that supports CMake targets
# Raylib: We'll fetch and build a specific release to guarantee reproducibility.
# Allow overriding the Raylib tag from outside (project user / CI) so builds are
# reproducible but flexible. Many raylib tags are prefixed with 'v' (eg: v4.5.0).
set(RAYLIB_TAG "master" CACHE STRING "Raylib Git tag/branch to fetch, or commit/branch name (default: 'master' to match bundled vendor headers)")
message(STATUS "Raylib tag configured: '${RAYLIB_TAG}' (override with -DRAYLIB_TAG=...))")

# Normalize the requested tag: try with/without 'v' prefix and fallback to a safe default.
set(_raylib_candidate "${RAYLIB_TAG}")
if(NOT DEFINED _raylib_candidate OR "${_raylib_candidate}" STREQUAL "")
    set(_raylib_candidate "v5.0")
endif()
message(STATUS "Raylib tag configured: requested='${RAYLIB_TAG}' - using '${_raylib_candidate}'")

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG ${_raylib_candidate}
)
# Configure some raylib-specific defaults before making it available
# Make sure we don't compile examples and tests from raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "Do not build raylib examples" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Do not build raylib tests" FORCE)
# Build a static lib to avoid runtime DLL problems on Windows by default
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# Make raylib available
FetchContent_MakeAvailable(raylib)

# Sanity: check if the fetched raylib matches the bundled vendor header
if(EXISTS "${CMAKE_SOURCE_DIR}/vendor/include/raylib.h" AND EXISTS "${raylib_SOURCE_DIR}/src/raylib.h")
    # Read vendor header and extract RAYLIB_VERSION line, then extract version
    file(READ "${CMAKE_SOURCE_DIR}/vendor/include/raylib.h" _vendor_rh)
    string(FIND "${_vendor_rh}" "#define RAYLIB_VERSION" _vendor_pos)
    if(NOT ${_vendor_pos} EQUAL -1)
        string(REGEX MATCH "#define RAYLIB_VERSION[^\n]*" _vendor_line "${_vendor_rh}")
        string(REGEX REPLACE ".*#define RAYLIB_VERSION[[:space:]]+\"([^\"]+)\".*" "\\1" VENDOR_RAYLIB_VERSION "${_vendor_line}")
    else()
        set(VENDOR_RAYLIB_VERSION "unknown")
    endif()
    file(READ "${raylib_SOURCE_DIR}/src/raylib.h" _fetched_rh)
    string(FIND "${_fetched_rh}" "#define RAYLIB_VERSION" _fetched_pos)
    if(NOT ${_fetched_pos} EQUAL -1)
        string(REGEX MATCH "#define RAYLIB_VERSION[^\n]*" _fetched_line "${_fetched_rh}")
        string(REGEX REPLACE ".*#define RAYLIB_VERSION[[:space:]]+\"([^\"]+)\".*" "\\1" FETCHED_RAYLIB_VERSION "${_fetched_line}")
    else()
        set(FETCHED_RAYLIB_VERSION "unknown")
    endif()
    if(NOT VENDOR_RAYLIB_VERSION STREQUAL FETCHED_RAYLIB_VERSION)
        message(WARNING "Detected raylib version mismatch: bundled vendor header is '${VENDOR_RAYLIB_VERSION}' but the fetched raylib is '${FETCHED_RAYLIB_VERSION}'.\n\nThis causes missing/renamed API functions (e.g. IsSoundValid -> IsSoundReady) and undefined references when linking. To fix, set -DRAYLIB_TAG=main or -DRAYLIB_TAG=${VENDOR_RAYLIB_VERSION} to fetch a matching raylib or update the code to call the new function names.")
    endif()
endif()
# raylib exports the target 'raylib' - we'll use that name when linking.

# ----------------------------------------------------------------------------
# Source files (DRY: use GLOB_RECURSE but avoid hardcoding lists)
# ----------------------------------------------------------------------------
# We split the project into a library + executable to enforce single responsibility
# - The library (game_core) contains game logic, states, entities and helpers
# - The executable contains only the entry point 'main.cpp' and the linking

file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Exclude main.cpp from the library (it belongs to the executable)
set(MAIN_SOURCE "${CMAKE_SOURCE_DIR}/src/main.cpp")
set(GAME_CORE_SOURCES)
foreach(_src IN LISTS ALL_SRC)
    if (NOT _src STREQUAL MAIN_SOURCE)
        list(APPEND GAME_CORE_SOURCES ${_src})
    endif()
endforeach()

# ----------------------------------------------------------------------------
# Targets
# ----------------------------------------------------------------------------

# Static library: game_core
add_library(game_core STATIC ${GAME_CORE_SOURCES})

# Public headers and vendor includes
target_include_directories(game_core
    PUBLIC
        $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/vendor/include
)
# Enlazamos raylib con game_core y lo hacemos PUBLIC
# Esto significa: "Quien use game_core, también recibe raylib automáticamente"
target_link_libraries(game_core PUBLIC raylib)

# Enforce modern properties and warnings (good default set)
target_compile_features(game_core PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(game_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(game_core PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
endif()

# Executable target
# Executable target
add_executable(game ${MAIN_SOURCE})
set_target_properties(game PROPERTIES OUTPUT_NAME "game.bin")

if(UNIX AND NOT APPLE)
    # TRUCO DE FUERZA BRUTA PARA LINUX:
    # El orden importa en GCC. Enlazamos raylib ANTES y DESPUÉS de game_core
    # para asegurar que se encuentran todos los símbolos.
    target_link_libraries(game PRIVATE 
        raylib          # 1. Raylib primero
        game_core       # 2. Tu código (que usa Raylib)
        raylib          # 3. Raylib otra vez (para cerrar el ciclo de dependencias)
        m pthread dl rt X11 GL # 4. Librerías de sistema explícitas
    )
else()
    # En Windows el linker es más listo y esto basta
    target_link_libraries(game PRIVATE game_core raylib)
endif()

# Make sure the library that contains game logic pulls raylib in transitively.
# This guarantees the `game_core` static lib has the proper transitive dependency
# and consumers (like the `game` executable) don't have to explicitly link raylib.
target_link_libraries(game_core PUBLIC raylib)

# On Linux, ensure we explicitly add system libraries that raylib often requires
# (mathematics, threads, dynamic loader, real-time, X11, GL). This is intended
# to be a 'force-brute' fix for undefined reference errors when linking the final
# executable on certain toolchains/runners. We do this only for Linux (not macOS).
if(UNIX AND NOT APPLE)
    message(STATUS "Linking additional system libs for Linux: m pthread dl rt X11 GL")
    # We prefer to use Threads target where available for portability
    find_package(Threads REQUIRED)
    target_link_libraries(game PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)
    # Also add named libs to ensure transitive linking resolves all symbols
    target_link_libraries(game PRIVATE m pthread dl rt X11 GL)
endif()
# Executable-specific includes (private)
target_include_directories(game PRIVATE $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES> ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/vendor/include)

# If you need to link other system libraries (GL, X11) - raylib target usually handles this.

# ----------------------------------------------------------------------------
# Installation rules (used by CPack)
# ----------------------------------------------------------------------------
# Install the actual game binary with a different name
install(TARGETS game
  RUNTIME DESTINATION bin
)

# Create a wrapper script that runs the game from the correct directory
# We generate it at configure time so we can install it as a regular file
set(WRAPPER_CONTENT "#!/bin/bash
# Get script directory
DIR=\$( cd \"\$( dirname \"\${BASH_SOURCE[0]}\" )\" && pwd )

# Try standard install path
if [ -d \"\$DIR/../share/${PROJECT_NAME}/assets\" ]; then
    cd \"\$DIR/../share/${PROJECT_NAME}\"
# Try local path
elif [ -d \"\$DIR/assets\" ]; then
    cd \"\$DIR\"
fi

# Run game
exec \"\$DIR/game.bin\" \"\$@\"
")

file(WRITE "${CMAKE_BINARY_DIR}/game_wrapper" "${WRAPPER_CONTENT}")

# Install the wrapper script
install(PROGRAMS "${CMAKE_BINARY_DIR}/game_wrapper"
    DESTINATION bin
    RENAME game
)

# Install assets and map files (so packaged app contains resources)
install(DIRECTORY assets/ DESTINATION share/${PROJECT_NAME}/assets)
install(DIRECTORY doc/ DESTINATION share/${PROJECT_NAME}/doc)

# Optional: install global headers if you want to publish the library
#install(DIRECTORY include/ DESTINATION include)

# ----------------------------------------------------------------------------
# Packaging - CPack configuration is kept in a separate module (for SRP)
# ----------------------------------------------------------------------------
include(cmake/CPackConfig.cmake)

# ----------------------------------------------------------------------------
# Targets to make development comfortable
# ----------------------------------------------------------------------------
# 'run' target for convenience: build and run in one command
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target game --config $<CONFIG>
    COMMAND $<TARGET_FILE:game>
    DEPENDS game
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Output helpful messages in configure stage
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Raylib target available: ${raylib_SOURCE_DIR}")

# ----------------------------------------------------------------------------
# Developer notes:
# - Use `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` to configure
# - Then `cmake --build build --config Release --target package` to build and create packages
# - To locally run the app:
#   cmake --build build --target run
# ----------------------------------------------------------------------------
