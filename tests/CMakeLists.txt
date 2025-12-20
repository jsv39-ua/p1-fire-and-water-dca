add_executable(unit_tests test_suite.cpp)

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
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
include(GoogleTest)
gtest_discover_tests(unit_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})