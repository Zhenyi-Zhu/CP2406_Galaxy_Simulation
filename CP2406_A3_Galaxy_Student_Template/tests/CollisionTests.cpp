#include "Collisions.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}

bool close(double actual, double expected)
{
    return std::abs(actual - expected) <=
        1e-12 * std::max({1.0, std::abs(actual), std::abs(expected)});
}

void expectVector(const vec3& actual, const vec3& expected, const std::string& field)
{
    require(close(actual.x, expected.x) && close(actual.y, expected.y) &&
            close(actual.z, expected.z), field);
}

body makeBody(double mass, vec3 position = {}, vec3 velocity = {}, vec3 accel = {})
{
    return {position, velocity, accel, mass};
}

bool sameVector(const vec3& a, const vec3& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool sameBody(const body& a, const body& b)
{
    return a.mass == b.mass && sameVector(a.position, b.position) &&
        sameVector(a.velocity, b.velocity) && sameVector(a.accel, b.accel);
}

bool sameBodies(const std::vector<body>& a, const std::vector<body>& b)
{
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (!sameBody(a[i], b[i])) return false;
    return true;
}

bool sameStats(const CollisionStats& a, const CollisionStats& b)
{
    return a.attemptedPairs == b.attemptedPairs && a.validPairs == b.validPairs &&
        a.mergedPairs == b.mergedPairs && a.activeBodies == b.activeBodies;
}

void expectStats(const CollisionStats& actual, const CollisionStats& expected)
{
    require(sameStats(actual, expected), "unexpected collision statistics");
}

template <typename Exception, typename Action>
void expectThrows(Action action)
{
    try { action(); }
    catch (const Exception&) { return; }
    throw std::runtime_error("expected exception was not thrown");
}

struct Totals
{
    double mass{};
    vec3 weightedPosition{};
    vec3 momentum{};
    vec3 force{};
};

Totals totals(const std::vector<body>& bodies)
{
    Totals result;
    for (const auto& b : bodies)
    {
        if (b.mass <= 0) continue;
        result.mass += b.mass;
        result.weightedPosition = result.weightedPosition + b.position * b.mass;
        result.momentum = result.momentum + b.velocity * b.mass;
        result.force = result.force + b.accel * b.mass;
    }
    return result;
}

std::vector<body> sampleBodies()
{
    return {
        makeBody(2, {0, 0, 0}, {1, 2, 3}, {1, 0, -1}),
        makeBody(3, {0.2, 0, 0}, {-1, 4, 1}, {3, 2, 1}),
        makeBody(4, {0.4, 0, 0}, {2, -1, 3}, {1, 2, 3}),
        makeBody(5, {10, 0, 0}, {4, 1, -2}, {0, -1, 2}),
        makeBody(6, {10.2, 0, 0}, {-2, 2, 1}, {2, 1, -1}),
        makeBody(0)
    };
}

void handCalculatedMerge()
{
    body a = makeBody(2, {0, 0, 0}, {1, 2, 3}, {1, 0, -1});
    body b = makeBody(3, {2, 4, 6}, {-1, 4, 1}, {3, 2, 1});
    require(checkAndMergeCollision(a, b, 8), "pair should merge");
    require(close(a.mass, 5), "combined mass should be 5");
    expectVector(a.position, {1.2, 2.4, 3.6}, "centre of mass");
    expectVector(a.velocity, {-0.2, 3.2, 1.8}, "momentum-weighted velocity");
    expectVector(a.accel, {2.2, 1.2, 0.2}, "force-weighted acceleration");
    require(sameBody(b, body{}), "retired body should be cleared");
    std::cout << "  Hand case: mass=" << a.mass
              << ", position=(" << a.position.x << ',' << a.position.y << ',' << a.position.z
              << "), velocity=(" << a.velocity.x << ',' << a.velocity.y << ',' << a.velocity.z
              << "), retired mass=" << b.mass << '\n';
}

void selfPair()
{
    body a = makeBody(2, {1, 2, 3}, {4, 5, 6});
    const body original = a;
    require(!checkAndMergeCollision(a, a, 10), "self-pair should not merge");
    require(sameBody(a, original), "self-pair changed its body");
}

void retiredBodies()
{
    for (double mass : {0.0, -1.0})
    {
        body a = makeBody(2, {1, 0, 0}, {2, 0, 0});
        body b = makeBody(mass);
        const body oldA = a, oldB = b;
        require(!checkAndMergeCollision(a, b, 5), "retired second body");
        require(!checkAndMergeCollision(b, a, 5), "retired first body");
        require(sameBody(a, oldA) && sameBody(b, oldB), "retired pair mutated");
    }
}

void outsideThreshold()
{
    body a = makeBody(2), b = makeBody(3, {0, 0, 2});
    const body oldA = a, oldB = b;
    require(!checkAndMergeCollision(a, b, 1), "distance must include z");
    require(sameBody(a, oldA) && sameBody(b, oldB), "far pair mutated");
}

void exactThreshold()
{
    body a = makeBody(2), b = makeBody(3, {3, 4, 0});
    require(checkAndMergeCollision(a, b, 5), "exact positive threshold must merge");
}

void justOutsideThreshold()
{
    body a = makeBody(2), b = makeBody(3, {std::nextafter(5.0, 6.0), 0, 0});
    require(!checkAndMergeCollision(a, b, 5), "just outside threshold must not merge");
}

void zeroThreshold()
{
    body a = makeBody(2), b = makeBody(3);
    require(!checkAndMergeCollision(a, b, 0), "zero must disable coincident merge");
    require(a.mass == 2 && b.mass == 3, "disabled merge changed mass");
}

void invalidThresholds()
{
    for (double value : {-1.0, std::numeric_limits<double>::infinity(),
                         std::numeric_limits<double>::quiet_NaN()})
    {
        body a = makeBody(2), b = makeBody(3);
        const body oldA = a, oldB = b;
        expectThrows<std::invalid_argument>([&] { checkAndMergeCollision(a, b, value); });
        require(sameBody(a, oldA) && sameBody(b, oldB), "invalid threshold mutated pair");
        std::vector<body> empty;
        expectThrows<std::invalid_argument>([&] { sampleAndMergeCollisions(empty, value, 0, 2406); });
    }
}

void invalidBodyData()
{
    const double infinity = std::numeric_limits<double>::infinity();
    for (int field = 0; field < 4; ++field)
    {
        body a = makeBody(2), b = makeBody(3);
        if (field == 0) b.mass = infinity;
        if (field == 1) b.position.x = infinity;
        if (field == 2) b.velocity.y = infinity;
        if (field == 3) b.accel.z = infinity;
        const body oldA = a, oldB = b;
        expectThrows<std::invalid_argument>([&] { checkAndMergeCollision(a, b, 1); });
        require(sameBody(a, oldA) && sameBody(b, oldB), "invalid data mutated pair");
    }
}

void massOverflow()
{
    body a = makeBody(std::numeric_limits<double>::max()), b = a;
    const body oldA = a, oldB = b;
    expectThrows<std::overflow_error>([&] { checkAndMergeCollision(a, b, 1); });
    require(sameBody(a, oldA) && sameBody(b, oldB), "overflow must leave pair unchanged");
}

void zeroAttempts()
{
    auto bodies = sampleBodies();
    const auto original = bodies;
    expectStats(sampleAndMergeCollisions(bodies, 1, 0, 2406), {0, 0, 0, 5});
    require(sameBodies(bodies, original), "zero attempts mutated collection");
}

void tinyCollections()
{
    std::vector<body> empty;
    expectStats(sampleAndMergeCollisions(empty, 1, 100, 2406), {0, 0, 0, 0});
    std::vector<body> one{makeBody(2)};
    expectStats(sampleAndMergeCollisions(one, 1, 100, 2406), {0, 0, 0, 1});
    require(one[0].mass == 2, "one-body mass changed");
}

void rareCollisions()
{
    std::vector<body> bodies{makeBody(2), makeBody(3, {10, 0, 0}), makeBody(4, {20, 0, 0})};
    const auto original = bodies;
    const auto result = sampleAndMergeCollisions(bodies, 0.001, 5000, 2406);
    require(result.attemptedPairs == 5000, "rare case must finish requested attempts");
    require(result.validPairs > 0 && result.validPairs < 5000, "exclude self-pairs");
    require(result.mergedPairs == 0 && result.activeBodies == 3, "rare case totals");
    require(sameBodies(bodies, original), "non-colliding data changed");
}

void disabledSamplingPass()
{
    std::vector<body> bodies{makeBody(2), makeBody(3)};
    const auto original = bodies;
    const auto result = sampleAndMergeCollisions(bodies, 0, 1000, 2406);
    require(result.attemptedPairs == 1000 && result.validPairs > 0 &&
            result.validPairs < 1000 && result.mergedPairs == 0 && result.activeBodies == 2,
            "disabled merging still counts attempts and eligible pairs");
    require(sameBodies(bodies, original), "disabled pass mutated data");
}

void retiredSelections()
{
    std::vector<body> bodies{makeBody(2), makeBody(0), makeBody(-1)};
    expectStats(sampleAndMergeCollisions(bodies, 5, 1000, 2406), {1000, 0, 0, 1});
    bodies[0].mass = 0;
    expectStats(sampleAndMergeCollisions(bodies, 5, 250, 2406), {250, 0, 0, 0});
}

void seededRepetition()
{
    auto first = sampleBodies(), second = first;
    const auto a = sampleAndMergeCollisions(first, 0.5, 2000, 2406);
    const auto b = sampleAndMergeCollisions(second, 0.5, 2000, 2406);
    require(sameStats(a, b) && sameBodies(first, second), "fixed seed must repeat all results");
    require(a.attemptedPairs == 2000 && a.mergedPairs == 3 && a.activeBodies == 2,
            "two compact groups should leave two survivors in this fixed-seed test");
    std::cout << "  Seed 2406 repeat: attempts=" << a.attemptedPairs << ", valid=" << a.validPairs
              << ", merged=" << a.mergedPairs << ", active=" << a.activeBodies
              << "; both runs identical\n";
}

void conservationAndStableStorage()
{
    auto bodies = sampleBodies();
    const auto before = totals(bodies);
    const auto size = bodies.size();
    body* const storage = bodies.data();
    const auto result = sampleAndMergeCollisions(bodies, 100, 2000, 2406);
    const auto after = totals(bodies);
    require(result.activeBodies == 1 && result.mergedPairs == 4, "high threshold merge count");
    require(close(before.mass, after.mass), "pass mass conservation");
    expectVector(after.weightedPosition, before.weightedPosition, "pass centre-of-mass conservation");
    expectVector(after.momentum, before.momentum, "pass momentum conservation");
    expectVector(after.force, before.force, "pass force contribution");
    require(bodies.size() == size && bodies.data() == storage, "retirement invalidated storage");
    require(result.mergedPairs <= result.validPairs && result.validPairs <= result.attemptedPairs,
            "statistics ordering");
}

void retiredBodyCannotMergeAgain()
{
    body a = makeBody(2), b = makeBody(3), c = makeBody(4);
    require(checkAndMergeCollision(a, b, 1), "initial merge");
    require(!checkAndMergeCollision(b, c, 1), "retired body must not merge again");
    require(a.mass == 5 && b.mass == 0 && c.mass == 4, "repeat retirement mass");
}
}

int main()
{
    struct Test { const char* name; void (*run)(); };
    const Test tests[] = {
        {"hand-calculated 3D merge", handCalculatedMerge},
        {"self-pair unchanged", selfPair},
        {"retired bodies ignored", retiredBodies},
        {"distance outside threshold", outsideThreshold},
        {"exact positive threshold", exactThreshold},
        {"just outside threshold", justOutsideThreshold},
        {"zero threshold disables merging", zeroThreshold},
        {"invalid thresholds", invalidThresholds},
        {"invalid body data", invalidBodyData},
        {"mass overflow without mutation", massOverflow},
        {"zero attempts", zeroAttempts},
        {"empty and one-body vectors", tinyCollections},
        {"bounded rare-collision pass", rareCollisions},
        {"bounded disabled-collision pass", disabledSamplingPass},
        {"retired selections excluded from valid pairs", retiredSelections},
        {"seeded repetition", seededRepetition},
        {"pass conservation and stable storage", conservationAndStableStorage},
        {"no second merge of retired body", retiredBodyCannotMergeAgain}
    };
    int passed = 0;
    for (const auto& test : tests)
    {
        try { test.run(); }
        catch (const std::exception& error)
        {
            std::cerr << "FAIL: " << test.name << ": " << error.what() << '\n';
            return 1;
        }
        ++passed;
        std::cout << "PASS: " << test.name << '\n';
    }
    std::cout << "PASS: " << passed << " collision test cases\n";
    return 0;
}
