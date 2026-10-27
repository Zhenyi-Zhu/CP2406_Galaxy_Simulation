#ifndef CSV_REPORTER_H
#define CSV_REPORTER_H

#include "Simulation.h"

#include <string>

void writeExperimentCsv(
    const std::string& file,
    const std::string& name,
    const SimulationConfig& config,
    const ExperimentResult& result);

#endif // CSV_REPORTER_H
