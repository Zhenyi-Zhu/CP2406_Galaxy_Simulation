#ifndef SIMULATION_CONFIG_H
#define SIMULATION_CONFIG_H
#include <cstddef>
#include <cstdint>
#include <string>
struct SimulationConfig {
    std::size_t bodyCount{2048};
    double systemSizeAU{10.0};
    int steps{20};
    double dtSeconds{1024000.0};
    double collisionThresholdAU{0.00001};
    std::size_t collisionSamples{1000};
    std::uint32_t seed{2406};
    std::string outputCsv{"experiment.csv"};
    void validate() const;
};
SimulationConfig parseArguments(int argc, char** argv);
std::string helpText();
#endif
