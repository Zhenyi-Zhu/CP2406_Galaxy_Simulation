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
endif()
