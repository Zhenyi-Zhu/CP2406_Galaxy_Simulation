#include "Collisions.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace
{
void validateThreshold(double thresholdAU)
{
    if (!std::isfinite(thresholdAU) || thresholdAU < 0)
    {
        throw std::invalid_argument(
            "collision threshold must be finite and non-negative");
    }
}

bool isFinite(const vec3& value)
{
    return std::isfinite(value.x) && std::isfinite(value.y) &&
           std::isfinite(value.z);
}

void validateActiveBody(const body& value)
{
    if (!std::isfinite(value.mass) || !isFinite(value.position) ||
        !isFinite(value.velocity) || !isFinite(value.accel))
    {
        throw std::invalid_argument("active collision body must have finite data");
    }
}

std::size_t countActiveBodies(const std::vector<body>& bodies)
{
    return static_cast<std::size_t>(std::count_if(
        bodies.begin(), bodies.end(),
        [](const body& value) { return value.mass > 0; }));
}
}

bool checkAndMergeCollision(body& a, body& b, double thresholdAU)
{
    validateThreshold(thresholdAU);
    // Zero disables merging, even for coincident bodies. Keep vector slots stable:
    // retirement must not invalidate vector indices or references during a pass.
    if (&a == &b || a.mass <= 0 || b.mass <= 0 || thresholdAU == 0)
    {
        return false;
    }

    validateActiveBody(a);
    validateActiveBody(b);
    const vec3 separation = a.position - b.position;
    const double distance = std::hypot(separation.x, separation.y, separation.z);
    if (distance > thresholdAU)
    {
        return false;
    }

    const double totalMass = a.mass + b.mass;
    if (!std::isfinite(totalMass) || totalMass <= 0)
    {
        throw std::overflow_error("merged mass must be finite and positive");
    }

    // Normalised mass weights implement the centre-of-mass and momentum equations
    // without first multiplying a large mass by every position/velocity component.
    const double weightA = a.mass / totalMass;
    const double weightB = b.mass / totalMass;
    const vec3 position = a.position * weightA + b.position * weightB;
    const vec3 velocity = a.velocity * weightA + b.velocity * weightB;
    // The simulation applies acceleration after merging in this step. Preserve
    // the combined force contribution instead of retaining only a's acceleration.
    const vec3 acceleration = a.accel * weightA + b.accel * weightB;
    if (!isFinite(position) || !isFinite(velocity) || !isFinite(acceleration))
    {
        throw std::overflow_error("merged body state must be finite");
    }

    // Commit only after all calculations succeed, preserving both inputs on error.
    a.mass = totalMass;
    a.position = position;
    a.velocity = velocity;
    a.accel = acceleration;
    b = body{}; // mass zero: ignored by sampling, tree insertion, motion and renderer.
    return true;
}

CollisionStats sampleAndMergeCollisions(
    std::vector<body>& bodies,
    double thresholdAU,
    std::size_t attempts,
    std::uint32_t seed)
{
    validateThreshold(thresholdAU);
    CollisionStats statistics;
    // No valid index distribution exists for an empty vector; one body also
    // cannot form a distinct pair. Zero attempts is an explicit no-sampling mode.
    if (bodies.size() < 2 || attempts == 0)
    {
        statistics.activeBodies = countActiveBodies(bodies);
        return statistics;
    }

    std::mt19937 generator(seed);
    std::uniform_int_distribution<std::size_t> index(0, bodies.size() - 1);
    for (std::size_t attempt = 0; attempt < attempts; ++attempt)
    {
        ++statistics.attemptedPairs;
        const std::size_t indexA = index(generator);
        const std::size_t indexB = index(generator);
        // Rejected draws still consume an attempt. Never retry until a merge occurs.
        if (indexA == indexB || bodies[indexA].mass <= 0 || bodies[indexB].mass <= 0)
        {
            continue;
        }

        ++statistics.validPairs;
        if (checkAndMergeCollision(bodies[indexA], bodies[indexB], thresholdAU))
        {
            ++statistics.mergedPairs;
        }
    }

    statistics.activeBodies = countActiveBodies(bodies);
    return statistics;
}
