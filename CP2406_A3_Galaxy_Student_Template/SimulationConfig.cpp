#include "SimulationConfig.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace
{
// Check the whole token before conversion: stoull alone accepts minus signs
// and stops at the first non-digit character.
unsigned long long readInteger(const std::string& value,
                               const std::string& option,
                               unsigned long long maximum)
{
    if (value.empty() || !std::all_of(value.begin(), value.end(),
        [](unsigned char c) { return c >= '0' && c <= '9'; }))
    {
        throw std::invalid_argument(option + " requires a non-negative integer");
    }

    unsigned long long number;
    try
    {
        number = std::stoull(value);
    }
    catch (const std::out_of_range&)
    {
        throw std::invalid_argument("value out of range for " + option);
    }
    if (number > maximum)
    {
        throw std::invalid_argument("value out of range for " + option);
    }
    return number;
}

double readNumber(const std::string& value, const std::string& option)
{
    const std::string message = option + " requires a finite number";
    if (value.empty() || std::any_of(value.begin(), value.end(),
        [](unsigned char c) { return std::isspace(c) != 0; }))
    {
        throw std::invalid_argument(message);
    }

    std::size_t consumed = 0;
    double number;
    try
    {
        number = std::stod(value, &consumed);
    }
    catch (const std::invalid_argument&)
    {
        throw std::invalid_argument(message);
    }
    catch (const std::out_of_range&)
    {
        throw std::invalid_argument("value out of range for " + option);
    }
    if (consumed != value.size() || !std::isfinite(number))
    {
        throw std::invalid_argument(message);
    }
    return number;
}
}

std::string helpText()
{
    return
        "Galaxy simulation options (use --option VALUE):\n"
        "  --bodies N                 At least 2 (default: 2048)\n"
        "  --system-size AU           Finite, positive (default: 10)\n"
        "  --steps N                  At least 1 (default: 20)\n"
        "  --dt SECONDS               Finite, positive (default: 1024000)\n"
        "  --collision-threshold AU   Finite, non-negative (default: 0.00001)\n"
        "  --collision-samples N      Non-negative integer (default: 1000)\n"
        "  --seed N                   Integer 0..4294967295 (default: 2406)\n"
        "  --output FILE              Non-blank CSV path (default: experiment.csv)\n"
        "  --help                     Show this help without running\n"
        "Integer values use decimal digits only. Repeated options use the last value.\n"
        "Example: Galaxy --bodies 200 --steps 20 --seed 2406 --output baseline.csv";
}

void SimulationConfig::validate() const
{
    if (outputCsv.empty() || std::all_of(outputCsv.begin(), outputCsv.end(),
        [](unsigned char c) { return std::isspace(c) != 0; }))
    {
        throw std::invalid_argument("--output requires a non-blank file path");
    }

    if (bodyCount < 2)
    {
        throw std::invalid_argument(
            "--bodies must be at least 2");
    }

    if (!std::isfinite(systemSizeAU) || systemSizeAU <= 0)
    {
        throw std::invalid_argument(
            "--system-size must be finite and positive");
    }

    if (steps < 1)
    {
        throw std::invalid_argument(
            "--steps must be at least 1");
    }

    if (!std::isfinite(dtSeconds) || dtSeconds <= 0)
    {
        throw std::invalid_argument(
            "--dt must be finite and positive");
    }

    if (!std::isfinite(collisionThresholdAU) || collisionThresholdAU < 0)
    {
        throw std::invalid_argument(
            "--collision-threshold must be finite and non-negative");
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
            if (argumentIndex + 1 >= argc ||
                std::string(argv[argumentIndex + 1]).rfind("--", 0) == 0)
            {
                throw std::invalid_argument(
                    "missing value for " + argument);
            }

            return std::string(argv[++argumentIndex]);
        };

        if (argument == "--bodies")
        {
            config.bodyCount = static_cast<std::size_t>(readInteger(
                readValue(), argument, std::numeric_limits<std::size_t>::max()));
        }
        else if (argument == "--system-size")
        {
            config.systemSizeAU = readNumber(readValue(), argument);
        }
        else if (argument == "--steps")
        {
            config.steps = static_cast<int>(readInteger(
                readValue(), argument, std::numeric_limits<int>::max()));
        }
        else if (argument == "--dt")
        {
            config.dtSeconds = readNumber(readValue(), argument);
        }
        else if (argument == "--collision-threshold")
        {
            config.collisionThresholdAU = readNumber(readValue(), argument);
        }
        else if (argument == "--collision-samples")
        {
            config.collisionSamples = static_cast<std::size_t>(readInteger(
                readValue(), argument, std::numeric_limits<std::size_t>::max()));
        }
        else if (argument == "--seed")
        {
            config.seed = static_cast<std::uint32_t>(
                readInteger(readValue(), argument,
                            std::numeric_limits<std::uint32_t>::max()));
        }
        else if (argument == "--output")
        {
            config.outputCsv = readValue();
        }
        else if (argument == "--help")
        {
            throw HelpRequested{};
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

