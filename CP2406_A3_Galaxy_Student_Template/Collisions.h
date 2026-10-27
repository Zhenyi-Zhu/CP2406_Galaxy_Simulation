#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <vector>

struct CollisionStats
{
    std::size_t attemptedPairs{};
    std::size_t validPairs{};
    std::size_t mergedPairs{};
    std::size_t activeBodies{};
};

bool checkAndMergeCollision(
    body& a,
    body& b,
    double thresholdAU);

CollisionStats sampleAndMergeCollisions(
    std::vector<body>& bodies,
    double thresholdAU,
    std::size_t attempts,
    std::uint32_t seed);

#endif // COLLISIONS_H
