#include "CsvReporter.h"

#include <fstream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <stdexcept>

namespace
{
std::string csvText(const std::string& value)
{
    if (value.find_first_of(",\"\r\n") == std::string::npos)
    {
        return value;
    }

    std::string escaped = "\"";
    for (char character : value)
    {
        if (character == '"')
        {
            escaped += '"';
        }
        escaped += character;
    }
    return escaped + '"';
}
}

void writeExperimentCsv(
    const std::string& file,
    const std::string& name,
    const SimulationConfig& config,
    const ExperimentResult& result)
{
    config.validate();
    if (!std::isfinite(result.elapsedMs) || result.elapsedMs < 0 ||
        !std::isfinite(result.totalMass) || result.totalMass <= 0 ||
        !std::isfinite(result.barycentre.x) ||
        !std::isfinite(result.barycentre.y) ||
        !std::isfinite(result.barycentre.z))
    {
        throw std::invalid_argument("Cannot report non-finite or invalid experiment metrics");
    }

    std::ofstream output(file, std::ios::binary);
    if (!output)
    {
        throw std::runtime_error("Cannot open CSV output: " + file);
    }

    // Decimal points and sufficient digits are independent of the user's locale.
    output.imbue(std::locale::classic());
    output << std::setprecision(std::numeric_limits<double>::max_digits10);

    output
        << "experiment,bodies,system_size_au,steps,dt_seconds,"
           "collision_threshold_au,collision_samples,seed,output_file,elapsed_ms,"
           "attempted_pairs,valid_pairs,merged_pairs,active_bodies,total_mass_kg,"
           "barycentre_x_au,barycentre_y_au,barycentre_z_au,status\n"
        << csvText(name) << ','
        << config.bodyCount << ','
        << config.systemSizeAU << ','
        << config.steps << ','
        << config.dtSeconds << ','
        << config.collisionThresholdAU << ','
        << config.collisionSamples << ','
        << config.seed << ','
        << csvText(file) << ','
        << result.elapsedMs << ','
        << result.collisions.attemptedPairs << ','
        << result.collisions.validPairs << ','
        << result.collisions.mergedPairs << ','
        << result.activeBodies << ','
        << result.totalMass << ','
        << result.barycentre.x << ','
        << result.barycentre.y << ','
        << result.barycentre.z << ",completed\n";

    // Closing explicitly also detects errors delayed by stream buffering.
    output.close();
    if (!output)
    {
        throw std::runtime_error("Cannot write CSV output: " + file);
    }
}
