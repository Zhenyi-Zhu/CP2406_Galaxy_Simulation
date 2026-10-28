# CP2406 Galaxy Option - Student Template

This project is based on the lecturer-provided Galaxy starter template. The baseline project runs, but the assessed collision, reporting, rendering and experiment behaviour is still incomplete. Refer to the assignment document for the full requirements.

## Build and run on Windows

1. Open the `CP2406_A3_Galaxy_Student_Template` subfolder containing `main.cpp` and `CMakeLists.txt` in CLion.
2. Configure the MinGW toolchain and reload CMake. The project requires C++20.
3. Build the `Galaxy` target.
4. In the Galaxy run configuration, set the working directory to the folder containing `main.cpp`.
5. Use `--help` to display the available command-line options.

For the initial baseline run, use these program arguments:

```text
--bodies 200 --system-size 50 --steps 20 --dt 1024000 --collision-threshold 0 --collision-samples 0 --seed 2406 --output baseline.csv
```

This run disables collision sampling. The template produces 20 PPM frames in `images/` and a placeholder `baseline.csv`. If FFmpeg is available, it also creates `galaxy.mp4`. At 30 frames per second, the video lasts approximately 0.67 seconds.

## FFmpeg setup used for the baseline

For the tested Windows setup, copy `ffmpeg.exe` from the extracted FFmpeg Essentials build's `bin` folder into the configured working directory, beside `main.cpp`. This lets the template find FFmpeg without changing PATH. FFmpeg is a separate dependency and is not included in this repository.

Open the generated MP4 with a Windows media player. Keep `ffmpeg.exe`, generated frames, generated videos, IDE settings and build directories out of commits.

## Development record — 27 October 2026

- Updated CMake for the Windows build and confirmed the `Galaxy` target builds successfully.
- Verified that `--help` displays the command-line options.
- Ran the baseline with 200 bodies, 20 steps and seed 2406.
- The first run produced frames but could not find FFmpeg.
- The WinGet installation attempt failed during archive extraction. A manually extracted executable placed in the working directory resolved video generation.
- The subsequent run reported `Created galaxy.mp4` and exited with code 0. Playback was also checked successfully.
- Committed the CMake configuration, ignore rules and initial CSV output.

These checks verify the baseline execution and video pipeline; they do not yet verify collision correctness or complete the assessment experiments.

## Development record — 28 October 2026

- Added strict integer parsing: reject negative values, fractional counts, trailing characters and values outside the destination type's range.
- Added full-token floating-point parsing while retaining finite-value and domain checks. Scientific notation is supported.
- Detect missing option values, including when the next token is another option, and reject blank output paths.
- Expanded help with units, defaults and accepted ranges. A dedicated `HelpRequested` exception keeps help separate from real runtime errors.
- Added the `ConfigTests` target and CTest registration. The tests remain active in Release builds.
- AI-assisted verification in a Linux/GCC 13.3 environment passed 142 configuration checks and eight executable-level scenarios. A normal baseline still generated 20 frames, a placeholder CSV and an MP4. Windows/CLion verification of this update remains to be performed locally.

See [the configuration test record](CP2406_A3_Galaxy_Student_Template/TESTING.md) for evidence, limitations and local verification steps.

## Run the configuration tests

In CLion, reload CMake, select the `ConfigTests` target, then build and run it. It requires no FFmpeg and creates no simulation outputs. If the target is absent, enable `BUILD_TESTING` in the CMake profile.

Alternatively, from the `CP2406_A3_Galaxy_Student_Template` source subfolder with CMake on PATH:

```text
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

For a multi-configuration generator, add `--config Debug` to the build command and `-C Debug` to CTest. Avoid changing the generator in an existing build directory.

Integer arguments accept decimal digits only. Repeated options use the last successfully parsed value. Quote output paths that contain spaces. Paths beginning with `--` can be written with a directory prefix such as `./--results.csv`.

## Next development step

Implement collision merging and bounded random pair sampling, then test conservation, inactive bodies and threshold boundaries. Review initialisation for very small positive system sizes: the starter currently uses a fixed 0.1 AU inner radius. Complete CSV reporting and the required experiments afterwards.
