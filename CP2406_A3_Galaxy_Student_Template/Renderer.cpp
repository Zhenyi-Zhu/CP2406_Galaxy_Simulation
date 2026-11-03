#include "Renderer.h"

#include "Constants.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

void renderSnapshot(
    const std::vector<body>& bodies,
    const std::string& filename,
    double viewHalfAU)
{
    if (!std::isfinite(viewHalfAU) || viewHalfAU <= 0.0)
    {
        throw std::invalid_argument(
            "Renderer view size must be finite and positive");
    }

    std::vector<std::uint8_t> image(WIDTH * HEIGHT * 3, 0);

    for (const body& currentBody : bodies)
    {
        if (currentBody.mass <= 0.0)
        {
            continue;
        }

        const vec3& position = currentBody.position;

        if (!std::isfinite(currentBody.mass) ||
            !std::isfinite(position.x) ||
            !std::isfinite(position.y) ||
            !std::isfinite(position.z))
        {
            throw std::invalid_argument(
                "Renderer active body must have finite mass and position");
        }

        // Clip in world coordinates before converting to integer pixels.
        // Finite off-screen bodies are valid simulation data.
        if (position.x < -viewHalfAU || position.x > viewHalfAU ||
            position.y < -viewHalfAU || position.y > viewHalfAU)
        {
            continue;
        }

        // Clipping ensures each ratio is in [-1, 1], keeping the
        // resulting pixel coordinates within the image dimensions.
        const double pixelX =
            (position.x / viewHalfAU * 0.5 + 0.5) * (WIDTH - 1);
        const double pixelY =
            (position.y / viewHalfAU * 0.5 + 0.5) * (HEIGHT - 1);

        const int x = static_cast<int>(pixelX);
        const int y = static_cast<int>(pixelY);

        const std::size_t pixelIndex =
            3 * (static_cast<std::size_t>(y) * WIDTH +
                 static_cast<std::size_t>(x));

        image[pixelIndex] = 180;
        image[pixelIndex + 1] = 200;
        image[pixelIndex + 2] = 255;
    }

    // Validate and prepare the image before opening the destination,
    // so invalid input does not truncate an existing file.
    std::ofstream output(filename, std::ios::binary);
    if (!output)
    {
        throw std::runtime_error(
            "Cannot open PPM output: " + filename);
    }

    output << "P6\n" << WIDTH << ' ' << HEIGHT << "\n255\n";
    output.write(
        reinterpret_cast<const char*>(image.data()),
        static_cast<std::streamsize>(image.size()));

    // Some write failures are only reported when buffered data is flushed.
    output.close();
    if (!output)
    {
        throw std::runtime_error(
            "Cannot write PPM output: " + filename);
    }
}