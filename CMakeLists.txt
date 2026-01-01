cmake_minimum_required(VERSION 3.21)
project(p1-fire-and-water-dca VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_GAME_TESTS "Build game tests" OFF)

include(GNUInstallDirs)
include(FetchContent)

# Localización (GNU gettext)
# En Windows no suele venir libintl por defecto. Para no bloquear la build,
# hacemos Intl opcional y activable explícitamente.
option(USE_GETTEXT "Enable gettext/libintl based localization" ON)

set(HAVE_GETTEXT OFF)
if(USE_GETTEXT)
  if(WIN32)
    find_package(Intl QUIET)
  else()
    find_package(Intl REQUIRED)
  endif()

  if(Intl_FOUND)
    set(HAVE_GETTEXT ON)
  endif()
endif()

message(STATUS "Gettext/Intl enabled: ${USE_GETTEXT} (found: ${HAVE_GETTEXT})")


# Por defecto apuntamos a master; se puede fijar a un tag/commit concreto con -DRAYLIB_TAG=
set(RAYLIB_TAG "master" CACHE STRING "Raylib Git tag/branch")

message(STATUS "Raylib tag configured: '${RAYLIB_TAG}' (override with -DRAYLIB_TAG=...)")
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG ${RAYLIB_TAG}
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)


add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(assets/locales)

if(BUILD_GAME_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/CPackConfig.cmake")
  include("${CMAKE_SOURCE_DIR}/cmake/CPackConfig.cmake")
endif()