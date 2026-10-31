# Configuration validation — 28 October 2026

## Scope and environment

The automated build and executable-level checks below ran in Linux with GCC 13.3, C++20 and CMake 4.4.4. I separately ran the Windows/CLion checks recorded in the Windows section on 28 October 2026.

## Build and automated checks

- Full program and configuration tests compiled successfully with GCC using `-Wall -Wextra -Wpedantic` without diagnostics.
- A separate CMake Release build completed successfully.
- CTest: `configuration_validation` passed (1 test suite).
- `ConfigTests`: **142 checks passed**. Checks use explicit failures rather than `assert`, so Release builds retain them.

Coverage includes defaults, the original baseline command, minimum values, destination-type maxima (parsing only), scientific notation, quoted paths with spaces, repeated options, negative/fractional integer inputs, overflow, trailing characters, non-finite values, floating-point overflow/underflow, missing values, unknown options, blank output paths, help and direct configuration validation.

Maximum-count cases only parse arguments; they do not allocate large simulations. Numeric representability does not guarantee a feasible memory/runtime budget or physical stability.

## Executable-level checks

Each case below was run against the compiled Galaxy executable in a temporary working directory. Error/help cases preserved an existing `images/frame_9999.ppm` sentinel unchanged.

| Input or condition | Expected | Observed |
|---|---|---|
| `--bodies -1` | Reject negative count; exit 1 | Passed |
| `--bodies 200abc` | Reject partially numeric input; exit 1 | Passed |
| `--seed 4294967296` | Reject uint32 overflow; exit 1 | Passed |
| `--output --steps 2` | Report missing output value; exit 1 | Passed |
| `--system-size nan` | Reject non-finite size; exit 1 | Passed |
| `--help` | Print help; exit 0; do not simulate | Passed |
| A regular file named `images` blocks directory creation | Report actual filesystem failure; exit 1 | Passed |
| Original 200-body, 20-step baseline | Exit 0; create 20 frames, CSV and MP4 | Passed |

The baseline used:

```text
--bodies 200 --system-size 50 --steps 20 --dt 1024000 --collision-threshold 0 --collision-samples 0 --seed 2406 --output baseline.csv
```

Its CSV remained the template's placeholder (`experiment,status` followed by `baseline,baseline`). The MP4 was non-empty and FFmpeg reported success; this automated check does not replace visual inspection of the simulation.

## Windows verification — 28 October 2026

I ran the following checks in Windows/CLion. My evidence is the console output I recorded, except where I confirmed a result without supplying its individual output.

| Check | Observed result | Evidence |
|---|---|---|
| `ConfigTests.exe` | `PASS: 142 configuration checks`; exit 0 | Console output |
| `Galaxy.exe --bodies -1` | `Error: --bodies requires a non-negative integer`; exit 1 | Console output |
| `Galaxy.exe --bodies 200abc` | Expected rejection confirmed | I confirmed that all requested cases were correct; I did not supply this case's individual output |
| `Galaxy.exe --seed 4294967296` | `Error: value out of range for --seed`; exit 1 | Console output |
| `Galaxy.exe --output --steps 2` | `Error: missing value for --output`; exit 1 | Console output |
| `Galaxy.exe --help` | Expanded help, including defaults and ranges; exit 0 | Console output |
| Original 200-body, 20-step baseline | `Created 20 frames in images/.`, `Created galaxy.mp4`, `Completed baseline run` | Console output supplied earlier in this session |

These Windows results validate the local test executable, the selected error paths, help and the baseline output pipeline. The filesystem-error and sentinel-preservation results are from Linux; I have not separately confirmed those cases on Windows.

To repeat my checks, I can select `ConfigTests` in CLion and run without arguments, then switch to `Galaxy` and run each argument set separately. Afterwards, I will restore the baseline arguments. I can use console output or screenshots as evidence in my development log.

## Remaining work

This configuration suite does not verify collision correctness, complete statistics/CSV reporting, physical stability or assessment experiments. Collision implementation and Linux/Windows test results from 29 October are recorded in [COLLISION_TESTING.md](COLLISION_TESTING.md). Reporting verification from 30 October is recorded below. I also completed the Windows reporting tests and baseline CSV check on that date. The starter's fixed 0.1 AU inner sampling radius also needs review before using system sizes below 0.1 AU.


## Reporting verification — 30 October 2026

Environment: Linux, GCC 13.3, C++20, CMake 4.4.4, Release, `-Wall -Wextra -Wpedantic`. The build completed without compiler diagnostics. CTest passed 3/3 suites: 142 configuration checks, 18 collision cases and 10 reporting cases. I also ran all 10 reporting test cases successfully in Windows/CLion on 30 October, with exit code 0.

Source: `tests/ReportingTests.cpp`. Tests use explicit exceptions, including in Release builds.

| Case | Expected | Actual (Linux) |is this actual Linux run
|---|---|---|
| Known configuration and results | Exact 19-column schema, one header/row, every metric in its column | Passed |
| Locale and precision | Decimal point despite a comma-decimal global locale; adjacent doubles round-trip | Passed |
| Text escaping | Commas, quotes and embedded newline in a name; comma in output path | Decoded correctly |
| Repeat write | Identical input bytes produce identical report; replace rather than duplicate rows/headers | Passed |
| Missing parent directory | Throw on open failure | Passed |
| Unwritable destination | Throw for directory destination; also detect buffered `/dev/full` failure on Linux | Passed |
| Invalid metrics | Reject NaN barycentre, infinite mass and negative elapsed time before touching existing file | Passed; previous report preserved |
| Disabled collisions | 40 bodies, 3 steps, 500 samples/step, threshold 0 | 1500 attempts, 0 merges, 40 active; mass and barycentre match final states |
| Merging and repeatability | Same baseline with threshold 100; fresh initialization for repeat | 39 merges, 1 active; mass conserved; reports identical after excluding measured elapsed time |
| Zero samples | Same baseline, samples 0 | 0 attempts/valid/merges, 40 active; summary matches final states |

The integration baseline uses size 10 AU, dt 1000 seconds and seed 2406. Each summary is checked against independently accumulated final body data, ignoring retired bodies. Floating-point mass/barycentre checks use relative/absolute tolerance 1e-12. Seeded report comparisons hold the output path and experiment name fixed and exclude only elapsed time.

Executable checks also passed:

- Original 200-body, 20-step baseline: exit 0, 20 PPM frames, non-empty MP4, complete 19-column CSV. Counts were 0 attempts/valid/merges and 200 active. Total mass was approximately 4e29 kg; final barycentre approximately (-1.98495, -0.0526317, 0) AU. The tracked `baseline.csv` was subsequently replaced by my Windows baseline result from 30 October, with elapsed time 35.5432 ms.
- Two high-collision CLI runs: 40 bodies, 3 steps, size 10, dt 1000, threshold 100, 500 samples, seed 2406. Each reported 1500 attempts, 39 valid, 39 merged and 1 active. Numeric report fields matched exactly apart from elapsed time; filenames/experiment names differed by design.
- `--output missing/result.csv`: exit 1 with `Cannot open CSV output`; no successful-completion message.
- `--bodies -1`: still rejected with exit 1.

### Output semantics and limits

The CSV records one completed experiment per file and replaces any existing file at that path. Use distinct paths for separate experiments. The column order is fixed. Elapsed time includes simulation and PPM rendering, but excludes initialization, CSV writing and FFmpeg. `completed` is simulation/report status, not video status. Exceptions prevent a completed row for failed simulations or invalid metrics; a write failure may leave a partial file and is reported with exit code 1.

The controlled matrix was completed on 31 October; see experiment_matrix.csv and the comparison in README. Initial-to-final barycentre analysis, visual comparison and the small-system initialization issue remain open., not the final three-threshold/two-body-count assessment matrix. Runtime comparisons, initial/final centre-of-mass movement and visual interpretation still need the controlled experiments. The small-system initialization issue recorded above also remains open.

### Repeat on Windows

Pull the update, reload CMake, select `ReportingTests` and run with no arguments. Expected final line: `PASS: 10 reporting test cases`, exit 0. Temporary output is cleaned up and FFmpeg is not required. Then run the original Galaxy baseline and inspect `baseline.csv`; it should have a header plus one data row with 19 columns, `active_bodies=200` and `status=completed`.

## Windows reporting verification — 30 October 2026

I ran ReportingTests.exe in Windows/CLion. All 10 reporting cases passed with exit code 0.

I also ran the 200-body baseline with collision threshold 0 and collision samples 0. The program generated the video and completed with exit code 0. Its CSV contained 19 columns, zero attempted/valid/merged pairs, 200 active bodies, approximately 4e29 kg total mass and status completed. Elapsed time was 35.5432 ms.

## Controlled experiment verification — 31 October 2026

I completed six configurations covering 200/400 bodies and thresholds of 0/0.01/1 AU, then repeated the 400-body, 1 AU configuration.

Both repeated runs reported 20000 attempts, 19372 valid pairs, 10 merges and 390 active bodies. Total mass and all reported barycentre coordinates matched exactly. Elapsed time was excluded from the reproducibility comparison.

The seven original CSV files and experiment_matrix.csv preserve these results. The README contains the comparison. Third-parameter investigation, initial-to-final barycentre analysis and visual comparison remain to be completed.