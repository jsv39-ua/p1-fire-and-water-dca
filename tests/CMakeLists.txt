# Buscar Boost.Test
find_package(Boost 1.70 COMPONENTS unit_test_framework QUIET)

if(NOT Boost_FOUND)
    message(STATUS "Boost::unit_test_framework no encontrado")
    include(FetchContent)
    set(BOOST_INCLUDE_LIBRARIES test)
    
    FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP ON
    )
    
    FetchContent_MakeAvailable(Boost)

    # Como dependiendo de si instalamos Boost con FetchContent o no el paquete puede tener un nombre diferente,
    # lo comprobamos y creamos un alias para que funcione dando igual el metodo de instalacion
    if(TARGET boost_unit_test_framework)   
        if(NOT TARGET Boost::unit_test_framework)
            add_library(Boost::unit_test_framework ALIAS boost_unit_test_framework)
        endif()
        
    elseif(TARGET boost_test)
        if(NOT TARGET Boost::unit_test_framework)
            add_library(Boost::unit_test_framework ALIAS boost_test)
        endif()
    endif()
    
endif()

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