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