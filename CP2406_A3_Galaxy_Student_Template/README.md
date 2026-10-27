# CP2406 Galaxy Option - Student Template

This project is based on the lecturer-provided Galaxy starter template. The baseline project runs, but the assessed collision, reporting, rendering and experiment behaviour is still incomplete. Refer to the assignment document for the full requirements.

## Build and run on Windows

1. Open the folder containing `main.cpp` and `CMakeLists.txt` in CLion.
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

## Next development step

Review and strengthen command-line parsing and input validation in `SimulationConfig.cpp` and its header. Check valid inputs, missing values, unknown options, invalid numbers and boundary values, and record the actual test results.
