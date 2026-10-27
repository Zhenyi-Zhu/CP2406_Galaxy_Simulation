cmake_minimum_required(VERSION 3.16)
project(Galaxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(OpenCV_DIR "/opt/homebrew/Cellar/opencv/5.0.0_7/lib/cmake/opencv5")
find_package(OpenCV REQUIRED COMPONENTS core imgproc videoio dnn)

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
