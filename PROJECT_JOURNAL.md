# CP2406 Weekly Project Development Journal

- Student name: Zhenyi Zhu
- Student number: 14887350
- Project option: Galaxy Simulation
- Teaching period: 2026 TR3

## Weekly Journal Entry 1

Week / dates: 27–29 October 2026

Project option: Galaxy Simulation

Checkpoint / task focus: Windows setup, configuration validation
and collision implementation.

### 1. Goals for This Week

I planned to build the starter project in Windows/CLion, run a small
baseline simulation and verify the generated outputs. My next goals
were to improve command-line validation and check collision merging
with focused tests before running larger experiments.

### 2. Development Work Completed

| File / component | Function or area changed | Summary of my work |
|---|---|---|
| `CMakeLists.txt` | Windows build and test targets | I updated the build configuration and built the project in CLion. |
| `SimulationConfig.cpp/.h` | Parsing, validation and help | I incorporated full-input checks, numeric range checks, finite-value validation and clearer help text. |
| `main.cpp` | Help and error handling | I incorporated separate handling for help requests and actual errors. |
| `Collisions.cpp/.h` | Pair merging and bounded sampling | I incorporated mass-weighted merging, retired-body handling and a sampling loop bounded by attempts. |
| `tests/ConfigTests.cpp` and `tests/CollisionTests.cpp` | Automated checks | I ran the test executables on Windows and checked the reported results. |
| README and test records | Setup and verification evidence | I documented the baseline, dependency setup and completed tests. |

### 3. Evidence of Progress

- [27 October: Windows build and baseline](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/0fbab45e6a3790e183c4a52ba547187f78e65fb4).
- [28 October: configuration implementation and tests](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/46f49cfa46961187962af4d790e746e28d7f9758).
- [28 October: Windows verification record](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/aa45f53e4838e7f4259fd5be0758934265d6007f).
- `TESTING.md`: configuration checks and manual CLI results.
- `COLLISION_TESTING.md`: collision rules, expected results and
  the successful Windows run on 29 October.

### 4. Problem-Solving Record

The baseline produced 20 PPM frames, but video creation initially
failed because FFmpeg was unavailable. The WinGet attempts also
encountered installation problems. I manually downloaded and
extracted FFmpeg and placed its executable in the configured working
directory. The next run created `galaxy.mp4`, and I confirmed playback.

This separated a missing video-conversion dependency from the
simulation itself. I kept the executable and generated video out
of my commits.

I also addressed numeric parsing problems. A conversion can accept
a numeric prefix without checking the remaining characters, while
unsigned conversion needs explicit handling of negative input.
I incorporated whole-token validation and destination-range checks,
then tested the rejection paths in Windows.

### 5. Testing Performed

| Test / input | Expected result | Actual result | Outcome |
|---|---|---|---|
| Baseline: 200 bodies, 20 steps, seed 2406, zero collision samples | Simulation frames and video with FFmpeg available | 20 frames created; MP4 generated and playback confirmed | Pass |
| `ConfigTests` | All configuration checks pass | 142 checks passed; exit code 0 | Pass |
| `--bodies -1` | Reject a negative body count | Non-negative-integer error; exit code 1 | Pass |
| `--seed 4294967296` | Reject seed overflow | Out-of-range error; exit code 1 | Pass |
| `--output --steps 2` | Detect a missing output value | Missing-value error; exit code 1 | Pass |
| `--help` | Display help without running the simulation | Help displayed; exit code 0 | Pass |
| `CollisionTests` | Correct merging, boundaries, bounded sampling and repetition | All 18 cases passed in Windows; exit code 0 | Pass |

The hand-calculated merge produced mass 5, position (1.2, 2.4, 3.6)
and velocity (-0.2, 3.2, 1.8). The retired body had zero mass.

### 6. Generative AI and External Help Log

| Tool / person | Purpose or question | Suggestions used | How I checked the results |
|---|---|---|---|
| ChatGPT | Brainstorming ideas, correcting grammar, improving formatting, and giving suggestions | I used some of the suggested wording, grammar corrections, and formatting changes. | I reviewed all suggestions and kept only those I agreed with. The code, testing and results are my own work. |

### 7. Weekly Reflection and Next Step

I learned to check the simulation, frame output and video conversion
as separate stages. A video-generation failure does not necessarily
mean the simulation failed.

The collision tests also helped me distinguish an attempted pair
from a valid pair and a successful merge. Bounding the loop by
attempts allows it to finish even when collisions are rare or disabled.

Next, I will verify the complete CSV report and run controlled
experiments with different collision settings.

My declaration: I confirm that this entry accurately records my
progress and assistance received.

Student initials / signature: Zhenyi Zhu
Date: 29 October 2026


## Weekly Journal Entry 2

Week / dates: 30 October–1 November 2026

Project option: Galaxy Simulation

Checkpoint / task focus: CSV reporting, controlled experiments,
repeatability and initial-to-final barycentre comparison.

### 1. Goals for This Week

I planned to verify the complete experiment reports, compare
simulation parameters while controlling other settings, and check
seeded repeatability. I also aimed to record initial and final
barycentres and preserve evidence of the results.

### 2. Development Work Completed

| File / component | Function or area changed | Summary of my work |
|---|---|---|
| `CsvReporter.cpp` | Complete experiment output | I incorporated the 19-column reporter and verified the Windows baseline CSV. |
| Experiment CSV files | Controlled comparisons | I ran the three-threshold/two-body-count matrix, repeated one seeded configuration, and compared three sampling budgets. |
| `Simulation.h` and `Simulation.cpp` | Initial summary | I added initial mass and barycentre fields and populated them before the simulation loop. |
| `main.cpp` | Console summary | I added initial and final values and calculated barycentre displacement as final minus initial. |
| `tests/ReportingTests.cpp` | Initial-summary verification | I added a known-value two-body test to check that the initial summary remains stored after movement. |
| Documentation | Results and verification | I recorded the comparisons, test outcomes and remaining work. |

### 3. Evidence of Progress

- `experiment_matrix.csv`: seven runs covering two body counts,
  three thresholds and one repeated seeded configuration.
- `sampling_comparison.csv`: 500, 1000 and 2000 samples per step
  with the other simulation settings fixed.
- `BARYCENTRE_COMPARISON.md`: initial and final values for
  collisions enabled and disabled.
- `TESTING.md`: dated Windows test results.
- [Commit 7ca51da](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/7ca51da):
  initial summary recording, barycentre comparison and the new test.

### 4. Problem-Solving Record

My first CSV aggregation attempt failed because PowerShell was in
the outer repository directory. I checked the file locations,
changed to the inner project directory and successfully exported
the seven-row experiment matrix.

Later, rerunning the 2000-sample experiment with the same output
filename replaced its original timing. I preserved the new output
as `n200_samples2000_rerun.csv` and restored the committed original.
This kept the earlier comparison table consistent with its source
data while retaining the later verification run.

Git still reported a change after restoration. I checked the diff
while ignoring end-of-line whitespace and found no content difference.
The repository used LF and the working copy used CRLF. Normalizing
the file to LF removed the remaining modification.

### 5. Testing Performed

| Test / input | Expected result | Actual result | Outcome |
|---|---|---|---|
| Configuration and collision regression suites | Existing checks remain successful | 142 configuration checks and 18 collision cases passed | Pass |
| Reporting suite on 30 October | CSV and integration checks pass | 10 cases passed; exit code 0 | Pass |
| Reporting suite after the initial-summary addition | Existing cases and new case pass | 11 cases passed; exit code 0 | Pass |
| 500, 1000 and 2000 samples per step over 20 steps | 10000, 20000 and 40000 attempts | Attempt counts matched; 6, 10 and 13 merges observed | Pass for attempt counts |
| Repeated 400-body, 1 AU threshold run | Matching simulation results, excluding timing and file labels | Counts, mass and final barycentre matched | Pass |
| Known-value initial summary | Mass 5 and barycentre (2.2, 3.2, 4.2), preserved after movement | All assertions passed | Pass |
| Enabled/disabled collision comparison | Matching starting conditions and recorded displacement | Initial barycentres matched; both runs moved and reported unchanged total mass | Completed |

The initial barycentre in both comparison runs was approximately
(-1.933654258, -0.004992345, 0) AU. The displacement differences
between the enabled and disabled runs were approximately
2.04e-7 AU in x and 7.54e-7 AU in y.

### 6. Generative AI and External Help Log

| Tool / person | Purpose or question | Suggestions used | How I checked the results |
|---|---|---|---|
| ChatGPT | Brainstorming ideas, correcting grammar, improving formatting, and giving suggestions | I used some of the suggested wording, grammar corrections, and formatting changes. | I reviewed all suggestions and kept only those I agreed with. The code, testing and results are my own work. |

### 7. Weekly Reflection and Next Step

I learned that increasing the sampling budget can change the number
of observed merges. In these runs, more samples produced more merges,
but the single-run timings did not establish a reliable performance
trend.

Recording the initial barycentre made the displacement calculation
explicit. The disabled-collision run also showed movement, so I
cannot attribute all movement to collisions. This comparison alone
does not establish momentum conservation throughout the simulation.

I also learned to use separate output filenames when preserving
experiments, because a successful rerun can still overwrite evidence.

Next, I will investigate initialization for system sizes below
0.1 AU, compare the visual output, and continue the code review
and demonstration preparation.

My declaration: I confirm that this entry accurately records my
progress and assistance received.

Student initials / signature: Zhenyi Zhu
Date: 1 November 2026

## Weekly Journal Entry 3

Week / dates: [Complete at the next actual checkpoint]

Project option: Galaxy Simulation

Checkpoint / task focus: [Complete when work is performed]

### 1. Goals for This Week

[Specific planned functions, features and tests.]

### 2. Development Work Completed

| File / component | Function or area changed | Summary of my changes / assistance |
|---|---|---|
| [Complete] | [Complete] | [Complete] |

### 3. Evidence of Progress

Evidence filename / link: [Add at least one dated authentic output, test result, screenshot or saved version and explain it.]

### 4. Problem-Solving Record

[Problem, investigation, changes and reason for the selected approach.]

### 5. Testing Performed

| Test / input | Expected result | Actual result | Pass / fail and action |
|---|---|---|---|
| [Complete] | [Complete] | [Complete] | [Complete] |

### 6. Generative AI and External Help Log

| Tool / person | Purpose or question | Suggestion used or rejected | How I checked / changed it |
|---|---|---|---|
| [Complete] | [Complete] | [Complete] | [Complete] |

### 7. Weekly Reflection and Next Step

[I will describe what I learned, what I can explain or modify, and my next specific task.]

My declaration: I confirm that this entry accurately records my progress and assistance received.

Student initials / signature: ____________________    Date: ____________________

## Final Project Reflection

Project option: Galaxy Simulation

### 1. Development Summary

[I will summarize my development work and the assistance I used at the end of the project.]

### 2. Most Difficult Problem

[I will describe the problem and how I resolved it in my own words.]

### 3. Most Valuable Test

[Identify the test, expected result, actual result and why it was valuable.]

### 4. Code Understanding

[I will identify one function I can explain and modify, including its key decisions.]

### 5. Final AI-Use Summary

[I will summarize the tools I used, the work they supported, and how I verified, modified or rejected their output.]

### 6. Future Improvement

[Explain the next improvement and why it matters.]

### Final Submission Check

- [ ] I have completed and dated my journal entries.
- [ ] I have included evidence relating to the work I describe.
- [ ] I have accurately recorded the AI and external help I used.
- [ ] I have verified that my project builds from a clean configuration.
- [ ] I can explain and modify the submitted code.
- [ ] I have excluded build folders, caches and unnecessary generated files from my project ZIP.

Student signature: ____________________    Date: ____________________
