# Buscar Boost.Test
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Crear ejecutable de tests
add_executable(unit_tests test_suite.cpp)

# Enlazar con Boost.Test, game_core y raylib
target_link_libraries(unit_tests PRIVATE 
    Boost::unit_test_framework
    game_core
    raylib
)

# Includes adicionales
target_include_directories(unit_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/core/include/components
    ${CMAKE_SOURCE_DIR}/core/include/entities
    ${CMAKE_SOURCE_DIR}/core/include/states
    ${CMAKE_SOURCE_DIR}/core/include/systems
)

# Copiar assets para que los tests puedan acceder a ellos
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Registrar el test en CTest
add_test(NAME GameTestSuite COMMAND unit_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})