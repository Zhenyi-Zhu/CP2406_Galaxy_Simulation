# CP2406 Galaxy Simulation

I am developing this project from the lecturer-provided Galaxy starter template. The program supports validated configuration, seeded collision sampling, safe body merging and complete experiment CSV summaries.

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

This run disables collision sampling. The program produces 20 PPM frames in `images/` and a `baseline.csv` containing the configuration and measured results. If FFmpeg is available, it also creates `galaxy.mp4`. At 30 frames per second, the video lasts approximately 0.67 seconds.

## FFmpeg setup used for the baseline

For the tested Windows setup, copy `ffmpeg.exe` from the extracted FFmpeg Essentials build's `bin` folder into the configured working directory, beside `main.cpp`. This lets the template find FFmpeg without changing PATH. FFmpeg is a separate dependency and is not included in this repository.

Open the generated MP4 with a Windows media player. Keep `ffmpeg.exe`, generated frames, generated videos, IDE settings and build directories out of commits.

## Development record — 27 October 2026

- I updated CMake and confirmed that the `Galaxy` target built successfully in Windows.
- I verified that `--help` displayed the command-line options.
- I ran the baseline with 200 bodies, 20 steps and seed 2406.
- The first run produced frames but could not find FFmpeg.
- The WinGet installation attempt failed during archive extraction. A manually extracted executable placed in the working directory resolved video generation.
- The subsequent run reported `Created galaxy.mp4` and exited with code 0. I also confirmed successful playback.
- I committed the CMake configuration, ignore rules and initial CSV output.

These checks verify the baseline execution and video pipeline; they do not yet verify collision correctness or complete the assessment experiments.

## Development record — 28 October 2026

- I incorporated integer parsing that rejects negative values, fractional counts, trailing characters and values outside the destination type's range.
- I incorporated full-token floating-point parsing with finite-value and domain checks. Scientific notation is supported.
- My validation detects missing option values, including when the next token is another option, and rejects blank output paths.
- I incorporated expanded help with units, defaults and accepted ranges. A dedicated `HelpRequested` exception keeps help separate from real runtime errors.
- I incorporated the `ConfigTests` target and CTest registration. The tests remain active in Release builds.
- Linux/GCC 13.3 verification passed 142 configuration checks and eight executable-level scenarios. A normal baseline generated 20 frames, a placeholder CSV and an MP4. I then ran all 142 configuration checks and the selected CLI cases successfully in Windows/CLion; I recorded the results in TESTING.md.

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

## Development record — 29 October 2026

- I incorporated implementations of `checkAndMergeCollision` and attempt-bounded `sampleAndMergeCollisions`.
- Positive thresholds include exact-boundary distances; threshold zero disables merging and zero samples skips sampling.
- Preserved mass, centre-of-mass position and linear momentum during merging; retained vector slots and cleared retired state. Acceleration is mass-weighted for the following integration update.
- I incorporated the `CollisionTests` suite. Linux verification completed successfully. All 18 focused cases passed, including fixed-seed repetition and conservation. The existing 142 configuration checks also passed.
- The Linux 40-body integration checks verified repeatability and conserved mass with merging enabled and disabled. I ran `CollisionTests` in Windows/CLion on 29 October: all 18 cases passed with exit code 0. The seed-2406 demonstration reported 2000 attempts, 101 valid pairs, 3 merges and 2 active bodies in both runs.

## Run collision tests

Reload CMake, select `CollisionTests` and run without arguments. Expected final line: `PASS: 18 collision test cases`. CTest also runs this suite. See [collision test evidence](CP2406_A3_Galaxy_Student_Template/COLLISION_TESTING.md) for rules, inputs, expected/actual results and environment limits.

The seeded engine/distribution repeats for the same starting data and build. Exact random index sequences are not promised across different standard-library implementations.

## Development record — 30 October 2026

- I incorporated the complete CSV reporter and console statistics, replacing the placeholder row.
- I retained deterministic column order, decimal points independent of locale, full double precision and proper quoting of text fields. Output failures now propagate to the normal error handler.
- I updated the final summary to count active bodies directly and promote mass before weighted-position multiplication.
- Linux/GCC 13.3 Release verification passed all three CTest suites: 142 configuration checks, 18 collision cases and 10 reporting cases. The baseline and repeated high-collision CLI runs also passed. My Windows verification of this reporting update is pending.

## Experiment output

Each run replaces the selected `--output` file with a header and one completed-experiment row. Use a different filename for each experiment to preserve comparisons. The experiment name comes from the output filename without its extension.

The 19 columns contain the experiment name; body count, system size, steps, dt, collision threshold, samples and seed; output path; elapsed milliseconds; attempted/valid/merged pairs; final active count, total mass and barycentre x/y/z; and status.

- Collision counts are totals across all simulation steps. Samples is the requested attempt count **per step**.
- Positions and system/threshold sizes are in AU, mass in kg and dt in seconds.
- `elapsed_ms` includes simulation, summary calculation and PPM output. It excludes initialisation, CSV writing and FFmpeg conversion; it is not a pure collision benchmark.
- `completed` describes a completed simulation report, not video conversion. An unavailable FFmpeg still leaves the CSV and frames; an output-file failure exits with code 1.
- Seeded comparisons exclude elapsed time and, when different output filenames are used, experiment name/output path. Exact repeatability applies within the same build/standard library.
- The tracked `baseline.csv` is the actual Linux/GCC verification output from 30 October; timings will vary on another run.

Reload CMake and run `ReportingTests` without arguments. Expected final line: `PASS: 10 reporting test cases`. It uses temporary files and frames, cleans them up and does not need FFmpeg. See [the reporting test record](CP2406_A3_Galaxy_Student_Template/TESTING.md#reporting-verification--30-october-2026).

## Assessment documents

- [Code review — Appendix A working draft](CP2406_A3_Galaxy_Student_Template/CODE_REVIEW.md)
- [Development journal — Appendix B working draft](CP2406_A3_Galaxy_Student_Template/PROJECT_JOURNAL.md)

## Next development step

I will verify the reporting update in Windows and run the controlled comparison matrix (at least three thresholds and two body counts, plus one repeated seeded configuration). I will review initialisation for system sizes below the starter's fixed 0.1 AU inner radius, then finish my code review, journal and 3–5 minute demonstration.
