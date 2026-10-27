#include "Renderer.h"

#include "Constants.h"

#include <cstdint>
#include <fstream>
#include <vector>

void renderSnapshot(
    const std::vector<body>& bodies,
    const std::string& filename,
    double viewHalfAU)
{
    std::vector<std::uint8_t> image(WIDTH * HEIGHT * 3, 0);

    for (const body& currentBody : bodies)
    {
        if (currentBody.mass <= 0)
        {
            continue;
        }

        const int x = static_cast<int>(
            (currentBody.position.x / viewHalfAU * 0.5 + 0.5) *
            (WIDTH - 1));
        const int y = static_cast<int>(
            (currentBody.position.y / viewHalfAU * 0.5 + 0.5) *
            (HEIGHT - 1));

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        {
            const std::size_t pixelIndex =
                3 * (static_cast<std::size_t>(y) * WIDTH + x);
            image[pixelIndex] = 180;
            image[pixelIndex + 1] = 200;
            image[pixelIndex + 2] = 255;
        }
    }

    std::ofstream output(filename, std::ios::binary);
    output << "P6\n" << WIDTH << ' ' << HEIGHT << "\n255\n";
    output.write(
        reinterpret_cast<const char*>(image.data()),
        static_cast<std::streamsize>(image.size()));
}
