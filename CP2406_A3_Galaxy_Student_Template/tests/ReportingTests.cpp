#include "CsvReporter.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}

void near(double actual, double expected, const std::string& message)
{
    require(std::isfinite(actual) &&
            std::abs(actual - expected) <= 1e-12 * std::max(1.0, std::abs(expected)),
            message);
}

struct TemporaryDirectory
{
    std::filesystem::path path = std::filesystem::temp_directory_path() /
        ("galaxy-report-tests-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    TemporaryDirectory() { std::filesystem::create_directory(path); }
    ~TemporaryDirectory()
    {
        std::error_code ignored;
        std::filesystem::remove_all(path, ignored);
    }
};

std::string read(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    require(input.good(), "read output");
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

// Independent small CSV reader: quoted commas, doubled quotes and embedded
// newlines stay in their field; Windows CRLF record endings are accepted.
std::vector<std::vector<std::string>> parseCsv(const std::string& text)
{
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> row;
    std::string field;
    bool quoted = false;
    for (std::size_t i = 0; i < text.size(); ++i)
    {
        const char ch = text[i];
        if (ch == '"')
        {
            if (quoted && i + 1 < text.size() && text[i + 1] == '"')
            {
                field += '"';
                ++i;
            }
            else quoted = !quoted;
        }
        else if (!quoted && (ch == ',' || ch == '\n'))
        {
            row.push_back(field);
            field.clear();
            if (ch == '\n') { rows.push_back(row); row.clear(); }
        }
        else if (ch != '\r' || quoted) field += ch;
    }
    require(!quoted && field.empty() && row.empty(), "complete CSV records");
    return rows;
}

struct DecimalComma : std::numpunct<char>
{
    char do_decimal_point() const override { return ','; }
};

struct LocaleGuard
{
    std::locale previous = std::locale();
    LocaleGuard() { std::locale::global(std::locale(std::locale::classic(), new DecimalComma)); }
    ~LocaleGuard() { std::locale::global(previous); }
};

template<class Function>
void rejects(Function function)
{
    bool threw = false;
    try { function(); } catch (const std::exception&) { threw = true; }
    require(threw, "expected an exception");
}

void checkSummary(const std::vector<body>& bodies, const ExperimentResult& result)
{
    double mass = 0;
    vec3 weighted{};
    std::size_t active = 0;
    for (const auto& current : bodies)
    {
        if (current.mass <= 0) continue;
        ++active;
        mass += current.mass;
        weighted = weighted + current.position * current.mass;
    }
    require(result.activeBodies == active && result.collisions.activeBodies == active,
            "active count from final states");
    near(result.totalMass, mass, "total mass from final states");
    near(result.barycentre.x, weighted.x / mass, "barycentre x");
    near(result.barycentre.y, weighted.y / mass, "barycentre y");
    near(result.barycentre.z, weighted.z / mass, "barycentre z");
    require(std::isfinite(result.elapsedMs) && result.elapsedMs >= 0, "elapsed time");
}
}

int main()
{
    try
    {
        TemporaryDirectory temporary;
        const auto file = temporary.path / "result.csv";
        SimulationConfig config;
        config.bodyCount = 5;
        config.systemSizeAU = 12.5;
        config.steps = 2;
        config.dtSeconds = 1000;
        config.collisionThresholdAU = 0.5;
        config.collisionSamples = 50;
        config.seed = 42;
        config.outputCsv = file.string();
        ExperimentResult result;
        result.collisions = {100, 72, 3, 2};
        result.activeBodies = 2;
        result.totalMass = 5;
        result.barycentre = {1.2, 2.4, 3.6};
        result.elapsedMs = 12.5;

        writeExperimentCsv(file.string(), "known", config, result);
        auto rows = parseCsv(read(file));
        const std::vector<std::string> expectedHeader{
            "experiment", "bodies", "system_size_au", "steps", "dt_seconds",
            "collision_threshold_au", "collision_samples", "seed", "output_file",
            "elapsed_ms", "attempted_pairs", "valid_pairs", "merged_pairs",
            "active_bodies", "total_mass_kg", "barycentre_x_au", "barycentre_y_au",
            "barycentre_z_au", "status"};
        require(rows.size() == 2 && rows[0] == expectedHeader && rows[1].size() == 19,
                "schema and row count");
        const auto& row = rows[1];
        require(row[0] == "known" && row[1] == "5" && row[2] == "12.5" &&
                row[3] == "2" && row[4] == "1000" && row[5] == "0.5" &&
                row[6] == "50" && row[7] == "42" && row[8] == file.string() &&
                row[9] == "12.5" && row[10] == "100" && row[11] == "72" &&
                row[12] == "3" && row[13] == "2" && row[14] == "5" &&
                row[18] == "completed", "configuration and result columns");
        require(std::stod(row[15]) == 1.2 && std::stod(row[16]) == 2.4 &&
                std::stod(row[17]) == 3.6, "barycentre columns");
        std::cout << "PASS: complete CSV schema and known values\n";

        result.barycentre.x = std::nextafter(1.0, 2.0);
        {
            LocaleGuard guard;
            writeExperimentCsv(file.string(), "precision", config, result);
        }
        rows = parseCsv(read(file));
        require(rows[1].size() == 19 && rows[1][2] == "12.5" &&
                std::stod(rows[1][15]) == result.barycentre.x, "locale and precision");
        std::cout << "PASS: locale-independent decimals and round-trip precision\n";

        const auto commaFile = temporary.path / "comma,name.csv";
        const std::string label = "case, \"quoted\"\nsecond line";
        writeExperimentCsv(commaFile.string(), label, config, result);
        rows = parseCsv(read(commaFile));
        require(rows.size() == 2 && rows[1].size() == 19 && rows[1][0] == label &&
                rows[1][8] == commaFile.string(), "text escaping");
        std::cout << "PASS: quoted names and output paths\n";

        writeExperimentCsv(file.string(), "repeat", config, result);
        const auto first = read(file);
        writeExperimentCsv(file.string(), "repeat", config, result);
        require(read(file) == first && parseCsv(first).size() == 2,
                "repeat output replaces file deterministically");
        std::cout << "PASS: deterministic replacement without duplicate headers\n";

        rejects([&] { writeExperimentCsv((temporary.path / "missing" / "x.csv").string(),
                                         "error", config, result); });
        std::cout << "PASS: missing output directory reports an error\n";
        rejects([&] { writeExperimentCsv(temporary.path.string(), "error", config, result); });
#ifndef _WIN32
        if (std::filesystem::exists("/dev/full"))
            rejects([&] { writeExperimentCsv("/dev/full", "error", config, result); });
#endif
        std::cout << "PASS: unwritable destination reports an error\n";

        auto invalid = result;
        invalid.barycentre.x = std::numeric_limits<double>::quiet_NaN();
        rejects([&] { writeExperimentCsv(file.string(), "invalid", config, invalid); });
        invalid = result;
        invalid.totalMass = std::numeric_limits<double>::infinity();
        rejects([&] { writeExperimentCsv(file.string(), "invalid", config, invalid); });
        invalid = result;
        invalid.elapsedMs = -1;
        rejects([&] { writeExperimentCsv(file.string(), "invalid", config, invalid); });
        require(read(file) == first, "invalid metrics must not truncate existing report");
        std::cout << "PASS: invalid metrics rejected before opening the file\n";

        config.bodyCount = 40;
        config.systemSizeAU = 10;
        config.steps = 3;
        config.collisionSamples = 500;
        config.seed = 2406;
        config.collisionThresholdAU = 0;
        auto bodies = initialiseBodies(config);
        double initialMass = 0;
        for (const auto& current : bodies) initialMass += current.mass;
        auto disabled = runExperiment(bodies, config, (temporary.path / "disabled").string());
        checkSummary(bodies, disabled);
        near(disabled.totalMass, initialMass, "disabled mass conservation");
        require(disabled.collisions.attemptedPairs == 1500 &&
                disabled.collisions.mergedPairs == 0 && disabled.activeBodies == 40,
                "disabled integration counts");
        std::cout << "PASS: disabled-collision integration summary\n";

        config.collisionThresholdAU = 100;
        bodies = initialiseBodies(config);
        auto merged = runExperiment(bodies, config, (temporary.path / "merged").string());
        checkSummary(bodies, merged);
        near(merged.totalMass, initialMass, "merged mass conservation");
        require(merged.collisions.attemptedPairs == 1500 &&
                merged.collisions.mergedPairs > 0 &&
                merged.activeBodies + merged.collisions.mergedPairs == config.bodyCount,
                "merged integration counts");
        auto repeatedBodies = initialiseBodies(config);
        auto repeated = runExperiment(repeatedBodies, config, (temporary.path / "repeat").string());
        checkSummary(repeatedBodies, repeated);
        // Timing is measured independently and is intentionally excluded.
        repeated.elapsedMs = merged.elapsedMs;
        writeExperimentCsv(file.string(), "seed-repeat", config, merged);
        const auto mergedCsv = read(file);
        writeExperimentCsv(file.string(), "seed-repeat", config, repeated);
        require(read(file) == mergedCsv, "all reported fields repeat except timing");
        std::cout << "PASS: merged integration and seeded report repetition\n";

        config.collisionSamples = 0;
        bodies = initialiseBodies(config);
        const auto unsampled = runExperiment(bodies, config, (temporary.path / "unsampled").string());
        checkSummary(bodies, unsampled);
        require(unsampled.collisions.attemptedPairs == 0 &&
                unsampled.collisions.validPairs == 0 &&
                unsampled.collisions.mergedPairs == 0 && unsampled.activeBodies == 40,
                "zero-sample integration summary");
        std::cout << "PASS: zero-sample integration summary\n"
                  << "PASS: 10 reporting test cases\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
