#include "Simulation.h"

#include "Bhtree.h"
#include "Constants.h"
#include "Renderer.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <random>
#include <sstream>
#include <utility>

namespace
{
Octant makeDynamicRoot(
    const std::vector<body>& bodies,
    double marginAU)
{
    double minimumX = 1.0e30;
    double minimumY = 1.0e30;
    double minimumZ = 1.0e30;
    double maximumX = -1.0e30;
    double maximumY = -1.0e30;
    double maximumZ = -1.0e30;

    for (const body& currentBody : bodies)
    {
        if (currentBody.mass <= 0.0)
        {
            continue;
        }

        minimumX = std::min(minimumX, currentBody.position.x);
        minimumY = std::min(minimumY, currentBody.position.y);
        minimumZ = std::min(minimumZ, currentBody.position.z);
        maximumX = std::max(maximumX, currentBody.position.x);
        maximumY = std::max(maximumY, currentBody.position.y);
        maximumZ = std::max(maximumZ, currentBody.position.z);
    }

    const vec3 centre{
        0.5 * (minimumX + maximumX),
        0.5 * (minimumY + maximumY),
        0.5 * (minimumZ + maximumZ)
    };

    double span = std::max({
        maximumX - minimumX,
        maximumY - minimumY,
        maximumZ - minimumZ
    });

    span = std::max(
        span + 2.0 * marginAU,
        2.0 * marginAU);

    return {
        centre.x,
        centre.y,
        centre.z,
        span
    };
}

void simulateOneStep(
    std::vector<body>& bodies,
    const SimulationConfig& config,
    int step,
    CollisionStats& accumulatedCollisions)
{
    Octant root = makeDynamicRoot(
        bodies,
        std::max(1.0, config.systemSizeAU));

    Bhtree tree(std::move(root));

    for (body& currentBody : bodies)
    {
        if (currentBody.mass > 0.0 &&
            tree.octant().contains(currentBody.position))
        {
            tree.insert(&currentBody);
        }
    }

    for (body& currentBody : bodies)
    {
        if (currentBody.mass <= 0.0)
        {
            continue;
        }

        currentBody.accel = {0, 0, 0};
        tree.interactInTree(&currentBody);
    }

    const CollisionStats stepStatistics =
        sampleAndMergeCollisions(
            bodies,
            config.collisionThresholdAU,
            config.collisionSamples,
            config.seed + static_cast<std::uint32_t>(step));

    accumulatedCollisions.attemptedPairs +=
        stepStatistics.attemptedPairs;
    accumulatedCollisions.validPairs +=
        stepStatistics.validPairs;
    accumulatedCollisions.mergedPairs +=
        stepStatistics.mergedPairs;
    accumulatedCollisions.activeBodies =
        stepStatistics.activeBodies;

    for (body& currentBody : bodies)
    {
        if (currentBody.mass <= 0.0)
        {
            continue;
        }

        currentBody.velocity =
            currentBody.velocity +
            currentBody.accel * config.dtSeconds;

        currentBody.position =
            currentBody.position +
            (currentBody.velocity * config.dtSeconds) / AU;
    }
}

void calculateSummary(
    const std::vector<body>& bodies,
    ExperimentResult& result)
{
    long double totalMass = 0;
    long double weightedX = 0;
    long double weightedY = 0;
    long double weightedZ = 0;

    for (const body& currentBody : bodies)
    {
        if (currentBody.mass <= 0.0)
        {
            continue;
        }

        totalMass += currentBody.mass;
        weightedX +=
            currentBody.mass * currentBody.position.x;
        weightedY +=
            currentBody.mass * currentBody.position.y;
        weightedZ +=
            currentBody.mass * currentBody.position.z;
    }

    result.activeBodies = result.collisions.activeBodies;
    result.totalMass = static_cast<double>(totalMass);

    if (totalMass > 0.0)
    {
        result.barycentre = {
            static_cast<double>(weightedX / totalMass),
            static_cast<double>(weightedY / totalMass),
            static_cast<double>(weightedZ / totalMass)
        };
    }
}
}

std::vector<body> initialiseBodies(
    const SimulationConfig& config)
{
    std::vector<body> bodies(config.bodyCount);
    std::mt19937 randomNumberGenerator(config.seed);

    std::uniform_real_distribution<double> radius(
        0.1,
        config.systemSizeAU);

    std::uniform_real_distribution<double> angle(
        0,
        2 * PI);

    const double massPerBody =
        0.2 * SOLAR_MASS /
        static_cast<double>(config.bodyCount);

    for (body& currentBody : bodies)
    {
        const double currentRadius =
            radius(randomNumberGenerator);
        const double currentAngle =
            angle(randomNumberGenerator);

        currentBody.position = {
            currentRadius * std::cos(currentAngle),
            currentRadius * std::sin(currentAngle),
            0
        };

        const double radiusMetres = currentRadius * AU;
        const double circularSpeed = std::sqrt(
            G * SOLAR_MASS /
            std::max(radiusMetres, 1.0));

        currentBody.velocity = {
            -circularSpeed * std::sin(currentAngle),
            circularSpeed * std::cos(currentAngle),
            0
        };

        currentBody.mass = massPerBody;
    }

    return bodies;
}

ExperimentResult runExperiment(
    std::vector<body>& bodies,
    const SimulationConfig& config,
    const std::string& framesDirectory)
{
    const auto startTime =
        std::chrono::steady_clock::now();

    ExperimentResult result;
    std::filesystem::create_directories(framesDirectory);

    for (int step = 0; step < config.steps; ++step)
    {
        simulateOneStep(
            bodies,
            config,
            step,
            result.collisions);

        std::ostringstream frameName;
        frameName
            << framesDirectory
            << "/frame_"
            << std::setw(4)
            << std::setfill('0')
            << step
            << ".ppm";

        renderSnapshot(
            bodies,
            frameName.str(),
            config.systemSizeAU);
    }

    calculateSummary(bodies, result);

    const auto finishTime =
        std::chrono::steady_clock::now();

    result.elapsedMs =
        std::chrono::duration<double, std::milli>(
            finishTime - startTime).count();

    return result;
}
