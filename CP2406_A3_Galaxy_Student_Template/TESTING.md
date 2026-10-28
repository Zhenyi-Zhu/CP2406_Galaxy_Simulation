# Configuration validation — 28 October 2026

## Scope and environment

The automated build and executable-level checks below were run by the AI coding assistant in Linux with GCC 13.3, C++20 and CMake 4.4.4. The separate Windows section records the student's subsequent CLion results supplied in the conversation on 28 October 2026.

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

The student ran the following checks in Windows/CLion. Evidence is the pasted console output, except where explicitly identified as a user confirmation.

| Check | Observed result | Evidence |
|---|---|---|
| `ConfigTests.exe` | `PASS: 142 configuration checks`; exit 0 | Console output |
| `Galaxy.exe --bodies -1` | `Error: --bodies requires a non-negative integer`; exit 1 | Console output |
| `Galaxy.exe --bodies 200abc` | Expected rejection confirmed | Student reported all requested cases correct; individual output not supplied |
| `Galaxy.exe --seed 4294967296` | `Error: value out of range for --seed`; exit 1 | Console output |
| `Galaxy.exe --output --steps 2` | `Error: missing value for --output`; exit 1 | Console output |
| `Galaxy.exe --help` | Expanded help, including defaults and ranges; exit 0 | Console output |
| Original 200-body, 20-step baseline | `Created 20 frames in images/.`, `Created galaxy.mp4`, `Completed baseline run` | Console output supplied earlier in this session |

These Windows results validate the local test executable, the selected error paths, help and the baseline output pipeline. The filesystem-error and sentinel-preservation scenarios above were checked in Linux; they have not been separately confirmed on Windows.

To repeat the checks, select `ConfigTests` in CLion and run without arguments, then switch to `Galaxy` and run each argument set separately. Restore the baseline arguments afterwards. Save actual console screenshots for the development log.

## Remaining work

Collision correctness, complete statistics/CSV reporting, physical stability and assessment experiments are not verified by this suite. The starter's fixed 0.1 AU inner sampling radius also needs review before using system sizes below 0.1 AU.
