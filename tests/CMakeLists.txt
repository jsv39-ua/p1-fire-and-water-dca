# Buscar Boost.Test en el sistema o descargarlo si no está disponible
find_package(Boost 1.70 COMPONENTS unit_test_framework QUIET)

if(Boost_FOUND)
    message(STATUS "Boost.Test encontrado en el sistema (versión ${Boost_VERSION})")
else()
    message(STATUS "Boost.Test no encontrado en el sistema")
    
    include(FetchContent)
    
    set(BOOST_INCLUDE_LIBRARIES test)

    FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.90.0/boost-1.90.0-cmake.7z
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        EXCLUDE_FROM_ALL
    )
    
    FetchContent_MakeAvailable(Boost)
endif()

# Crear ejecutable de tests
add_executable(unit_tests test_suite.cpp)

if(NOT WIN32 AND Boost_FOUND)
    target_compile_definitions(unit_tests PRIVATE BOOST_TEST_DYN_LINK)
endif()

# Enlazar con Boost.Test, la librería del juego y raylib
target_link_libraries(unit_tests PRIVATE 
    Boost::unit_test_framework
    game_core
    raylib
)

target_include_directories(unit_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/core/include/components
    ${CMAKE_SOURCE_DIR}/core/include/entities
    ${CMAKE_SOURCE_DIR}/core/include/states
    ${CMAKE_SOURCE_DIR}/core/include/systems
)

# Copiar assets para que los tests puedan acceder a los recursos
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

add_test(
    NAME GameTestSuite 
    COMMAND unit_tests 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)