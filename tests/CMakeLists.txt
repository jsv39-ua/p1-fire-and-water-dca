# tests/CMakeLists.txt

# Creamos el ejecutable de pruebas
add_executable(unit_tests test_suite.cpp)

# Enlazamos con GTest, tu librería y raylib
target_link_libraries(unit_tests PRIVATE 
    GTest::gtest_main 
    game_core
    raylib
)

# Includes
target_include_directories(unit_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/systems
    ${CMAKE_SOURCE_DIR}/vendor/include
)

# --- MAGIA PARA LOS ASSETS ---

# 1. Copiar la carpeta 'assets' del proyecto a la carpeta 'build'
#    Esto se ejecuta cuando haces 'cmake -B build'
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# 2. Registrar los tests en CTest y FORZAR el directorio de trabajo
#    Esto es CLAVE: Le dice al test que se ejecute desde 'build/', 
#    así cuando pidas "assets/config/...", los encontrará.
include(GoogleTest)
gtest_discover_tests(unit_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})