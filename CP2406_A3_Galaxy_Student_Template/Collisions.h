#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "Types.h"

#include <cstddef>
#include <cstdint>
#include <vector>

// A valid pair has distinct indices and two active bodies. Attempts include
// rejected same-index and retired-body draws; merges are a subset of valid pairs.
struct CollisionStats
{
    std::size_t attemptedPairs{};
    std::size_t validPairs{};
    std::size_t mergedPairs{};
    std::size_t activeBodies{};
};

// Positive thresholds merge at distance <= thresholdAU; zero disables merging.
// Active bodies have mass > 0; retired bodies retain their vector slot with mass 0.
bool checkAndMergeCollision(
    body& a,
    body& b,
    double thresholdAU);

// Fewer than two slots: zero attempts. Otherwise use exactly the requested
// attempts, including when merging is disabled or all bodies become retired.
// Repeatability is for the same input, seed and standard-library implementation.
CollisionStats sampleAndMergeCollisions(
    std::vector<body>& bodies,
    double thresholdAU,
    std::size_t attempts,
    std::uint32_t seed);

#endif // COLLISIONS_H

