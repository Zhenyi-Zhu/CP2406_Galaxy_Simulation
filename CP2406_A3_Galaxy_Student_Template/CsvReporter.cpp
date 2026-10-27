#include "CsvReporter.h"

#include <fstream>

void writeExperimentCsv(
    const std::string& file,
    const std::string& name,
    const SimulationConfig&,
    const ExperimentResult&)
{
    std::ofstream output(file);

    output
        << "experiment,status\n"
        << name
        << ",baseline\n";
}
