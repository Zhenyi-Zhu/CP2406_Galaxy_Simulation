#include "CsvReporter.h"
#include "Simulation.h"
#include "SimulationConfig.h"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iomanip>

namespace
{
void clearOldFrames()
{
    std::filesystem::create_directories("images");

    for (const auto& entry :
         std::filesystem::directory_iterator("images"))
    {
        const std::string filename =
            entry.path().filename().string();

        if (entry.is_regular_file() &&
            filename.rfind("frame_", 0) == 0 &&
            entry.path().extension() == ".ppm")
        {
            std::filesystem::remove(entry.path());
        }
    }
}

bool createVideo()
{
    return std::system(
        "ffmpeg -y -framerate 30 "
        "-i images/frame_%04d.ppm "
        "-c:v libx264 -pix_fmt yuv420p galaxy.mp4") == 0;
}
}

int main(int argc, char** argv)
{
    try
    {
        const SimulationConfig config = parseArguments(argc, argv);
        clearOldFrames();

        std::vector<body> bodies = initialiseBodies(config);
        const ExperimentResult result = runExperiment(
            bodies,
            config,
            "images");

        writeExperimentCsv(
            config.outputCsv,
            std::filesystem::path(config.outputCsv).stem().string(),
            config,
            result);

        std::cout
            << "Created " << config.steps
            << " frames in images/.\n"
            << "Collisions: attempted=" << result.collisions.attemptedPairs
            << ", valid=" << result.collisions.validPairs
            << ", merged=" << result.collisions.mergedPairs << '\n'
            << "Active bodies: " << result.activeBodies << '\n'
            << std::setprecision(17)
            << "Initial total mass (kg): "
            << result.initialTotalMass << '\n'
            << "Final total mass (kg): "
            << result.totalMass << '\n'
            << "Initial barycentre (AU): ("
            << result.initialBarycentre.x << ", "
            << result.initialBarycentre.y << ", "
            << result.initialBarycentre.z << ")\n"
            << "Final barycentre (AU): ("
            << result.barycentre.x << ", "
            << result.barycentre.y << ", "
            << result.barycentre.z << ")\n"
            << "Barycentre displacement (AU): ("
            << (result.barycentre.x - result.initialBarycentre.x) << ", "
            << (result.barycentre.y - result.initialBarycentre.y) << ", "
            << (result.barycentre.z - result.initialBarycentre.z) << ")\n"
            << "Elapsed (simulation + frames, ms): " << result.elapsedMs << '\n'
            << "Created " << config.outputCsv << '\n' << std::flush;

        if (createVideo())
        {
            std::cout << "Created galaxy.mp4\n";
        }
        else
        {
            std::cerr
                << "Frames were created, but FFmpeg was not found or failed.\n";
        }

        std::cout << "Completed experiment run\n";
        return 0;
    }
    catch (const HelpRequested&)
    {
        std::cout << helpText() << '\n';
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}

