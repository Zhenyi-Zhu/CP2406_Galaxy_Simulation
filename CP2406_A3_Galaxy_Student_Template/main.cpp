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
            "baseline",
            config,
            result);

        std::cout
            << "Created " << config.steps
            << " frames in images/.\n";

        if (createVideo())
        {
            std::cout << "Created galaxy.mp4\n";
        }
        else
        {
            std::cerr
                << "Frames were created, but FFmpeg was not found or failed.\n";
        }

        std::cout << "Completed baseline run\n";
        return 0;
    }
    catch (const std::runtime_error& error)
    {
        std::cout << error.what() << '\n';
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
