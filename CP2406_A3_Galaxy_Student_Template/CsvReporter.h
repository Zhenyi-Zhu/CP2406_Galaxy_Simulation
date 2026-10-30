#ifndef CSV_REPORTER_H
#define CSV_REPORTER_H

#include "Simulation.h"

#include <string>

// One header and one completed-experiment row; replaces the selected file.
// elapsedMs covers simulation and PPM rendering, excluding CSV and FFmpeg.
// Throws if metrics are invalid or opening/writing/closing the file fails.
void writeExperimentCsv(
    const std::string& file,
    const std::string& name,
    const SimulationConfig& config,
    const ExperimentResult& result);

#endif // CSV_REPORTER_H
