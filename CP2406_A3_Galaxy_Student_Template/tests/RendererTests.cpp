#include "Renderer.h"
#include "Constants.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace
{
namespace fs = std::filesystem;

void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

template<typename Exception, typename Function>
void expectException(Function action)
{
    try
    {
        action();
    }
    catch (const Exception&)
    {
        return;
    }

    throw std::runtime_error("Expected exception was not thrown");
}

struct TemporaryDirectory
{
    fs::path path;

    TemporaryDirectory()
    {
        const auto stamp =
            std::chrono::steady_clock::now().time_since_epoch().count();
        path = fs::temp_directory_path() /
               ("galaxy_renderer_tests_" + std::to_string(stamp));

        require(fs::create_directory(path),
                "Cannot create unique test directory");
    }

    ~TemporaryDirectory()
    {
        std::error_code error;
        fs::remove_all(path, error);
    }
};

body makeBody(double x, double y, double mass = 1.0)
{
    body result{};
    result.position = {x, y, 0.0};
    result.mass = mass;
    return result;
}

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    require(static_cast<bool>(input), "Cannot read test output");

    std::string contents{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()};

    require(!input.bad(), "Error reading test output");
    return contents;
}

// Expected pixels are supplied explicitly, without repeating
// the renderer's world-coordinate conversion.
std::string expectedImage(
    const std::vector<std::pair<int, int>>& pixels = {})
{
    const std::string header =
            "P6\n" + std::to_string(WIDTH) + " " +
            std::to_string(HEIGHT) + "\n255\n";

    std::string result = header;
    result.append(static_cast<std::size_t>(WIDTH) * HEIGHT * 3, '\0');

    for (const auto& [x, y] : pixels)
    {
        const std::size_t offset =
                header.size() +
                3 * (static_cast<std::size_t>(y) * WIDTH + x);

        result[offset] = static_cast<char>(180);
        result[offset + 1] = static_cast<char>(200);
        result[offset + 2] = static_cast<char>(255);
    }

    return result;
}
}

int main()
{
    try
    {
        TemporaryDirectory temporary;
        const fs::path output = temporary.path / "snapshot.ppm";
        int passed = 0;

        const auto test = [&](const char* name, auto action)
        {
            action();
            ++passed;
            std::cout << "PASS: " << name << '\n';
        };

        test("empty scene produces a complete black PPM", [&]
        {
            renderSnapshot({}, output.string(), 1.0);
            require(readFile(output) == expectedImage(),
                    "Incorrect PPM header, size or background");
        });

        test("centre and exact view boundaries map to expected pixels", [&]
        {
            const std::vector<body> bodies{
                makeBody(-1.0, -1.0),
                makeBody(1.0, -1.0),
                makeBody(-1.0, 1.0),
                makeBody(1.0, 1.0),
                makeBody(0.0, 0.0)
            };

            renderSnapshot(bodies, output.string(), 1.0);

            require(readFile(output) == expectedImage({
                {0, 0},
                {WIDTH - 1, 0},
                {0, HEIGHT - 1},
                {WIDTH - 1, HEIGHT - 1},
                {(WIDTH - 1) / 2, (HEIGHT - 1) / 2}
            }), "Incorrect boundary or centre pixels");
        });

        test("just-outside and extreme finite coordinates are clipped", [&]
        {
            const double outside = std::nextafter(1.0, 2.0);
            const double huge = std::numeric_limits<double>::max();

            const std::vector<body> bodies{
                makeBody(-outside, 0.0),
                makeBody(outside, 0.0),
                makeBody(0.0, -outside),
                makeBody(0.0, outside),
                makeBody(huge, 0.0),
                makeBody(-huge, 0.0),
                makeBody(0.0, huge),
                makeBody(0.0, -huge)
            };

            renderSnapshot(bodies, output.string(), 1.0);
            require(readFile(output) == expectedImage(),
                    "Off-screen body was drawn");
        });

        test("retired bodies are skipped", [&]
        {
            const double nan =
                    std::numeric_limits<double>::quiet_NaN();

            renderSnapshot({
                makeBody(0.0, 0.0, 0.0),
                makeBody(0.5, 0.5, -1.0),
                makeBody(nan, nan, 0.0)
            }, output.string(), 1.0);

            require(readFile(output) == expectedImage(),
                    "Retired body affected the image");
        });

        test("invalid view sizes are rejected without creating a file", [&]
        {
            const fs::path invalidOutput = temporary.path / "invalid.ppm";
            const double infinity =
                    std::numeric_limits<double>::infinity();
            const double nan =
                    std::numeric_limits<double>::quiet_NaN();

            for (double view : {0.0, -1.0, infinity, -infinity, nan})
            {
                expectException<std::invalid_argument>([&]
                {
                    renderSnapshot({}, invalidOutput.string(), view);
                });

                require(!fs::exists(invalidOutput),
                        "Invalid view created an output file");
            }
        });

        test("invalid active bodies preserve existing output", [&]
        {
            renderSnapshot({makeBody(0.0, 0.0)}, output.string(), 1.0);
            const std::string original = readFile(output);

            const double infinity =
                    std::numeric_limits<double>::infinity();
            const double nan =
                    std::numeric_limits<double>::quiet_NaN();

            std::vector<body> invalidBodies;
            for (double value : {infinity, nan})
            {
                body invalid = makeBody(0.0, 0.0);
                invalid.mass = value;
                invalidBodies.push_back(invalid);

                invalid = makeBody(0.0, 0.0);
                invalid.position.x = value;
                invalidBodies.push_back(invalid);

                invalid = makeBody(0.0, 0.0);
                invalid.position.y = value;
                invalidBodies.push_back(invalid);

                invalid = makeBody(0.0, 0.0);
                invalid.position.z = value;
                invalidBodies.push_back(invalid);
            }

            for (const body& invalid : invalidBodies)
            {
                expectException<std::invalid_argument>([&]
                {
                    renderSnapshot(
                        {makeBody(0.5, 0.5), invalid},
                        output.string(), 1.0);
                });

                require(readFile(output) == original,
                        "Invalid input changed the existing file");
            }
        });

        test("repeated rendering replaces output deterministically", [&]
        {
            const std::vector<body> bodies{makeBody(0.0, 0.0)};

            renderSnapshot(bodies, output.string(), 1.0);
            const std::string first = readFile(output);
            renderSnapshot(bodies, output.string(), 1.0);

            require(readFile(output) == first,
                    "Identical input produced different output");

            renderSnapshot({}, output.string(), 1.0);
            require(readFile(output) == expectedImage(),
                    "Replacement retained old pixels or appended data");
        });

        test("missing parent and directory destinations report errors", [&]
        {
            expectException<std::runtime_error>([&]
            {
                renderSnapshot(
                    {}, (temporary.path / "missing" / "image.ppm").string(),
                    1.0);
            });

            expectException<std::runtime_error>([&]
            {
                renderSnapshot({}, temporary.path.string(), 1.0);
            });
        });

        std::cout << "PASS: " << passed << " renderer test cases\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}