cmake_minimum_required(VERSION 3.16)
project(Galaxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Galaxy
        main.cpp
        Bhtree.cpp
        Collisions.cpp
        CsvReporter.cpp
        Octant.cpp
        Renderer.cpp
        Simulation.cpp
        SimulationConfig.cpp
)

include(CTest)
if(BUILD_TESTING)
    add_executable(ConfigTests tests/ConfigTests.cpp SimulationConfig.cpp)
    target_include_directories(ConfigTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME configuration_validation COMMAND ConfigTests)
    add_executable(CollisionTests tests/CollisionTests.cpp Collisions.cpp)
    target_include_directories(CollisionTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME collision_behaviour COMMAND CollisionTests)
    set_tests_properties(collision_behaviour PROPERTIES TIMEOUT 10)
    add_executable(ReportingTests tests/ReportingTests.cpp CsvReporter.cpp
            Simulation.cpp SimulationConfig.cpp Collisions.cpp Bhtree.cpp
            Octant.cpp Renderer.cpp)
    target_include_directories(ReportingTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME experiment_reporting COMMAND ReportingTests)
    set_tests_properties(experiment_reporting PROPERTIES TIMEOUT 30)
endif()
