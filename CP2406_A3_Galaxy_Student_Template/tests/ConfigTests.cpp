#include "SimulationConfig.h"

#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
int checks = 0;

void require(bool condition, const std::string& description)
{
    ++checks;
    if (!condition) throw std::runtime_error("FAILED: " + description);
}

SimulationConfig parse(std::vector<std::string> arguments)
{
    arguments.insert(arguments.begin(), "Galaxy");
    std::vector<char*> argv;
    for (auto& argument : arguments) argv.push_back(argument.data());
    return parseArguments(static_cast<int>(argv.size()), argv.data());
}

void rejects(const std::vector<std::string>& arguments, const std::string& message)
{
    try
    {
        parse(arguments);
    }
    catch (const std::invalid_argument& error)
    {
        require(std::string(error.what()).find(message) != std::string::npos,
                "error should identify " + message + ": " + error.what());
        return;
    }
    throw std::runtime_error("FAILED: input should be rejected: " + arguments.front());
}

void checkDirectValidation(const std::function<void(SimulationConfig&)>& change)
{
    SimulationConfig config;
    change(config);
    try { config.validate(); }
    catch (const std::invalid_argument&) { ++checks; return; }
    throw std::runtime_error("FAILED: direct config validation");
}
}

int main()
{
    try
    {
        const auto defaults = parse({});
        require(defaults.bodyCount == 2048 && defaults.steps == 20 &&
                defaults.seed == 2406 && defaults.systemSizeAU == 10 &&
                defaults.dtSeconds == 1024000 && defaults.collisionSamples == 1000 &&
                defaults.collisionThresholdAU == 0.00001 &&
                defaults.outputCsv == "experiment.csv", "default configuration");

        const auto baseline = parse({"--bodies", "200", "--system-size", "50",
            "--steps", "20", "--dt", "1024000", "--collision-threshold", "0",
            "--collision-samples", "0", "--seed", "2406", "--output", "baseline.csv"});
        require(baseline.bodyCount == 200 && baseline.steps == 20 &&
                baseline.systemSizeAU == 50 && baseline.dtSeconds == 1024000 &&
                baseline.collisionThresholdAU == 0 && baseline.collisionSamples == 0 &&
                baseline.seed == 2406 && baseline.outputCsv == "baseline.csv",
                "original baseline arguments");

        const auto minimum = parse({"--bodies", "2", "--steps", "1", "--seed", "0",
            "--collision-samples", "0", "--collision-threshold", "0"});
        require(minimum.bodyCount == 2 && minimum.steps == 1 && minimum.seed == 0 &&
                minimum.collisionSamples == 0 && minimum.collisionThresholdAU == 0,
                "minimum integer values and disabled collisions");
        require(parse({"--seed", "4294967295"}).seed == 4294967295U, "maximum seed");
        require(parse({"--steps", std::to_string(std::numeric_limits<int>::max())}).steps ==
                std::numeric_limits<int>::max(), "maximum steps: parse only");
        const auto sizeMaximum = std::to_string(std::numeric_limits<std::size_t>::max());
        require(parse({"--bodies", sizeMaximum}).bodyCount ==
                std::numeric_limits<std::size_t>::max(), "maximum body count: parse only");
        require(parse({"--collision-samples", sizeMaximum}).collisionSamples ==
                std::numeric_limits<std::size_t>::max(), "maximum sample count: parse only");
        require(parse({"--dt", "1.024e6"}).dtSeconds == 1024000, "scientific notation");
        require(parse({"--system-size", "2.5"}).systemSizeAU == 2.5, "decimal size");
        require(parse({"--output", "results with spaces.csv"}).outputCsv ==
                "results with spaces.csv", "quoted output path with spaces");
        require(parse({"--steps", "2", "--steps", "3"}).steps == 3, "last option wins");

        for (const std::string option : {"--bodies", "--steps", "--collision-samples", "--seed"})
        {
            for (const std::string value : {"-1", "+2", "2.5", "200abc", "abc", "", " 2", "2 ",
                                           "1e2", "18446744073709551616"})
                rejects({option, value}, option);
        }
        rejects({"--seed", "4294967296"}, "--seed");
        rejects({"--steps", std::to_string(
            static_cast<unsigned long long>(std::numeric_limits<int>::max()) + 1)}, "--steps");
        rejects({"--bodies", "0"}, "--bodies");
        rejects({"--bodies", "1"}, "--bodies");
        rejects({"--steps", "0"}, "--steps");

        for (const std::string option : {"--system-size", "--dt", "--collision-threshold"})
        {
            for (const std::string value : {"nan", "inf", "-inf", "NaN", "1e9999", "1e-9999",
                                           "12abc", "1.2.3", "abc", "", " 2", "2 ", "-1"})
                rejects({option, value}, option);
        }
        rejects({"--system-size", "0"}, "--system-size");
        rejects({"--dt", "0"}, "--dt");

        for (const std::string option : {"--bodies", "--steps", "--seed", "--collision-samples",
                                        "--system-size", "--dt", "--collision-threshold", "--output"})
        {
            rejects({option}, "missing value for " + option);
            rejects({option, "--steps", "2"}, "missing value for " + option);
        }
        rejects({"--output", ""}, "--output");
        rejects({"--output", " \t"}, "--output");
        rejects({"--unknown", "2"}, "unknown option: --unknown");
        rejects({"unexpected"}, "unknown option: unexpected");

        bool helpRequested = false;
        try { parse({"--help"}); }
        catch (const HelpRequested&) { helpRequested = true; }
        require(helpRequested, "distinct help request");
        for (const std::string option : {"--bodies", "--steps", "--seed", "--collision-samples",
                                        "--system-size", "--dt", "--collision-threshold", "--output", "--help"})
            require(helpText().find(option) != std::string::npos, "help documents " + option);

        for (double bad : {std::numeric_limits<double>::quiet_NaN(),
                           std::numeric_limits<double>::infinity(),
                           -std::numeric_limits<double>::infinity(), -1.0})
        {
            checkDirectValidation([bad](auto& c) { c.systemSizeAU = bad; });
            checkDirectValidation([bad](auto& c) { c.dtSeconds = bad; });
            checkDirectValidation([bad](auto& c) { c.collisionThresholdAU = bad; });
        }
        checkDirectValidation([](auto& c) { c.outputCsv = ""; });
        checkDirectValidation([](auto& c) { c.steps = 0; });
        checkDirectValidation([](auto& c) { c.bodyCount = 1; });
        std::cout << "PASS: " << checks << " configuration checks\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
