#ifndef SIMULATION_H
#define SIMULATION_H

#include "Collisions.h"
#include "SimulationConfig.h"
#include "Types.h"

#include <cstddef>
#include <string>
#include <vector>

struct ExperimentResult
{
    CollisionStats collisions{};
    std::size_t activeBodies{};
    double totalMass{};
    vec3 barycentre{};
    double elapsedMs{};
};

std::vector<body> initialiseBodies(const SimulationConfig& config);

ExperimentResult runExperiment(
    std::vector<body>& bodies,
    const SimulationConfig& config,
    const std::string& framesDirectory = "images");

#endif // SIMULATION_H
