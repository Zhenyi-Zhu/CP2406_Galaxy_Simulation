#include "SimulationConfig.h"

#include <stdexcept>
#include <string>

std::string helpText()
{
    return
        "Options: --bodies N --system-size AU --steps N "
        "--dt SECONDS --collision-threshold AU "
        "--collision-samples N --seed N --output FILE --help";
}

void SimulationConfig::validate() const
{
    if (bodyCount < 2)
    {
        throw std::invalid_argument(
            "body count must be at least 2");
    }

    if (systemSizeAU <= 0)
    {
        throw std::invalid_argument(
            "system size must be positive");
    }

    if (steps < 1)
    {
        throw std::invalid_argument(
            "steps must be at least 1");
    }

    if (dtSeconds <= 0)
    {
        throw std::invalid_argument(
            "time step must be positive");
    }

    if (collisionThresholdAU < 0)
    {
        throw std::invalid_argument(
            "collision threshold cannot be negative");
    }
}

SimulationConfig parseArguments(int argc, char** argv)
{
    SimulationConfig config;

    for (int argumentIndex = 1;
         argumentIndex < argc;
         ++argumentIndex)
    {
        const std::string argument = argv[argumentIndex];

        const auto readValue = [&]()
        {
            if (argumentIndex + 1 >= argc)
            {
                throw std::invalid_argument(
                    "missing value for " + argument);
            }

            return std::string(argv[++argumentIndex]);
        };

        if (argument == "--bodies")
        {
            config.bodyCount = std::stoull(readValue());
        }
        else if (argument == "--system-size")
        {
            config.systemSizeAU = std::stod(readValue());
        }
        else if (argument == "--steps")
        {
            config.steps = std::stoi(readValue());
        }
        else if (argument == "--dt")
        {
            config.dtSeconds = std::stod(readValue());
        }
        else if (argument == "--collision-threshold")
        {
            config.collisionThresholdAU = std::stod(readValue());
        }
        else if (argument == "--collision-samples")
        {
            config.collisionSamples = std::stoull(readValue());
        }
        else if (argument == "--seed")
        {
            config.seed = static_cast<std::uint32_t>(
                std::stoul(readValue()));
        }
        else if (argument == "--output")
        {
            config.outputCsv = readValue();
        }
        else if (argument == "--help")
        {
            throw std::runtime_error(helpText());
        }
        else
        {
            throw std::invalid_argument(
                "unknown option: " + argument);
        }
    }

    config.validate();
    return config;
}
