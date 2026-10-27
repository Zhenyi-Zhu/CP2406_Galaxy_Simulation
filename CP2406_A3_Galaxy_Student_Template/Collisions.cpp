#include "Collisions.h"

#include <stdexcept>

bool checkAndMergeCollision(
    body&,
    body&,
    double thresholdAU)
{
    if (thresholdAU < 0)
    {
        throw std::invalid_argument(
            "collision threshold cannot be negative");
    }

    return false;
}

CollisionStats sampleAndMergeCollisions(
    std::vector<body>& bodies,
    double thresholdAU,
    std::size_t attempts,
    std::uint32_t)
{
    if (thresholdAU < 0)
    {
        throw std::invalid_argument(
            "collision threshold cannot be negative");
    }

    CollisionStats statistics;
    statistics.attemptedPairs = attempts;

    for (const body& currentBody : bodies)
    {
        if (currentBody.mass > 0)
        {
            ++statistics.activeBodies;
        }
    }

    return statistics;
}
