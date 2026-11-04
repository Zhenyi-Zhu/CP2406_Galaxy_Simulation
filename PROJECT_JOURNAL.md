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

- [27 October: Windows build and baseline](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/0fbab45e6a3790e183c4a52ba547187f78e65fb4).
- [28 October: configuration implementation and tests](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/46f49cfa46961187962af4d790e746e28d7f9758).
- [28 October: Windows verification record](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/aa45f53e4838e7f4259fd5be0758934265d6007f).
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

My declaration: I confirm that this entry accurately records my progress and assistance received.

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
- [Commit 7ca51da](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/7ca51da):
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

My declaration: I confirm that this entry accurately records my progress and assistance received.

Student initials / signature: Zhenyi Zhu
Date: 1 November 2026

## Weekly Journal Entry 3

Week / dates: 2–4 November 2026

Project option: Galaxy Simulation

Checkpoint / task focus: Small-system initialization, visual
comparison, code review and clean build verification.

### 1. Goals for This Week

I planned to investigate initialization for small system sizes,
compare the visual results of three collision thresholds, update
my code review and verify the project using a fresh build directory.

### 2. Development Work Completed

| File / component | Function or area changed | Summary of my work |
|---|---|---|
| `Simulation.cpp` | Initialization radius | I incorporated an adaptive lower radius so that small positive system sizes have a valid sampling range. |
| `tests/ReportingTests.cpp` | Initialization checks | I incorporated and ran a test covering system sizes of 0.05, 0.1, 0.2 and 50 AU. |
| Visual comparison files | Final frames and experiment results | I ran three threshold configurations and saved their CSV reports and final-frame images. |
| `VISUAL_COMPARISON.md` | Results discussion | I documented the identical disabled/rare frames and the small differences in the high-threshold frame. |
| `CODE_REVIEW.md` | Architecture, evidence and challenge | I updated the explanations to reflect the current implementation and completed verification. |
| `TESTING.md` | Build and verification records | I recorded all four Windows CTest suites, the extracted-archive verification and the subsequent 13-case reporting regression run. |
| `Renderer.cpp` and `tests/RendererTests.cpp` | Rendering safety | I incorporated input validation, clipping before integer conversion and output-error checks, then verified all eight renderer cases on Windows. |
| `Bhtree.h/.cpp` and `tests/ReportingTests.cpp` | Ownership and coincident positions | I incorporated non-copyable ownership and tree-only aggregation for coincident bodies, with compile-time checks and a runtime regression case. |

### 3. Evidence of Progress

- [Small-system fix and regression test](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/5520e02e9d0088ed0a6e84cb999867097a34b868).
- [Visual comparison](CP2406_A3_Galaxy_Student_Template/VISUAL_COMPARISON.md):
  saved final frames and results for three collision thresholds.
- [Small-system CSV](CP2406_A3_Galaxy_Student_Template/small_system_check.csv):
  the completed 40-body, 0.05 AU integration run.
- [Code review update](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/a33066a7a6381c3f52583bd6ad80d9274afb693d).
- [Clean build verification](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/437227cfd06034d6fcfe21314be53807f0da9061).
- [Test-target correction and complete CTest verification](https://github.com/Zhenyi-Zhu/CP2406_Galaxy_Simulation/commit/31208f43dd8c8615b4fc039da441c62a03e3352e).

### 4. Problem-Solving Record

The configuration accepted finite positive system sizes, but
initialization used a fixed lower radius of 0.1 AU. For a system
size of 0.05 AU, this lower bound exceeded the upper bound.

I incorporated the expression
`std::min(0.1, config.systemSizeAU * 0.5)` for the lower radius.
This gives a range of 0.025–0.05 AU for the small-system check
while preserving the original 0.1 AU lower bound for systems
of at least 0.2 AU.

I checked initialization separately from later movement through
the regression test, then ran a short simulation to verify that
the configuration completed successfully.

The visual comparison also required care because each simulation
reused `images/` and `galaxy.mp4`. I saved separate comparison
files and checked the final PPM hashes. The disabled and rare
frames matched, while the high-threshold frame differed.

On 3 November, all eight RendererTests cases passed on Windows
with exit code 0. They check actual PPM bytes, boundary clipping,
retired bodies, invalid input, preservation of existing output,
deterministic replacement and destination-opening errors.

Four static_assert checks in ReportingTests.cpp verify that
Bhtree cannot be copied or moved. These checks run at compile
time and are separate from the runtime test count.

Galaxy and ReportingTests built successfully after the ownership
change. All 12 reporting cases then passed with exit code 0.
The dated verification record is in TESTING.md.

On 4 November, the repository review identified that RendererTests.cpp
had also been included in the CollisionTests target. Both test files
define main(), so they need separate executables. I removed the
renderer test source from CollisionTests and retained its independent
RendererTests target.

An initial PowerShell build failed without a specific compiler
diagnostic. RendererTests then compiled and linked successfully
inside CLion. After adding the MinGW bin directory to the terminal
PATH, the all-target build reported that no work was required.
This did not establish the cause of the earlier compilation failure.

I then ran all four suites through CTest. All passed, with zero
failures. This showed why checking individual targets alone was
not enough to verify the complete test configuration.

The final review also identified repeated subdivision when two
bodies occupied exactly the same position. A separate Linux
reproduction crashed. I incorporated a leaf-node check that keeps
the shared position and combines gravitational mass within the tree.

This does not retire simulation bodies or count as a collision.
The regression test checks the force contribution to a third body
and a short simulation with collisions disabled. All 13 reporting
cases passed in my Windows CleanCheck run.

### 5. Testing Performed

| Test / input | Expected result | Actual result | Outcome |
|---|---|---|---|
| Initialization at 0.05, 0.1, 0.2 and 50 AU | Correct body count and radius bounds, finite velocities and positive masses | Initialization checks passed within the 12-case reporting suite | Pass |
| 40 bodies, 0.05 AU, 3 steps, timestep 1 second, collisions disabled | Successful short run with 40 active bodies | Completed; initial and final reported mass matched; CSV, frames and video generated | Pass |
| Visual comparison at thresholds 0, 0.01 and 1 AU | Record outcomes with other settings fixed | Observed 0, 0 and 10 merges, leaving 200, 200 and 190 active bodies | Completed |
| Final-frame hash comparison | Establish whether saved frames are identical | Disabled and rare hashes matched; high-threshold hash differed | Completed |
| Galaxy build in `cmake-build-cleancheck` | Compile and link using a fresh build directory | Build completed successfully | Pass |
| Test suites under CleanCheck | All checks pass with exit code 0 | 142 configuration checks, 18 collision cases and 12 reporting cases passed | Pass |
| Renderer safety tests | Correct image bytes, clipping and error handling | All 8 cases passed; exit code 0 | Pass |
| Bhtree ownership restrictions | Copy and move operations unavailable | Four compile-time assertions passed during the ReportingTests build | Pass |
| Reporting regression after ownership changes | Existing integration behaviour remains correct | All 12 cases passed; exit code 0 | Pass |
| Full CTest run on 4 November | All four registered suites pass | 4/4 passed, zero failures; reported time 1.33 seconds | Pass |
| Extracted archive from commit 77df575 | Build independently of the working repository and pass all registered tests | All 27 build steps completed; CTest passed 4/4 suites in 0.87 seconds | Pass |
| Extracted-archive smoke run | Produce one completed CSV row and three frames | Exit 0; 40 active bodies; zero collision counts; CSV and three frames created; missing FFmpeg reported separately | Pass for simulation and reporting |
| Coincident-body regression | Avoid repeated subdivision, preserve gravitational contribution and retain independent simulation bodies | New case passed; all 13 reporting cases passed in Windows with exit code 0 | Pass |

I also verified an archive exported from commit 77df575 in a separate
directory. Its fresh build, four CTest suites and simulation smoke
check passed. FFmpeg was unavailable in that isolated run, so it
did not verify video conversion.

That archive predates the coincident-body correction. I subsequently
verified all 13 reporting cases on Windows.

### 6. Generative AI and External Help Log

| Tool / person | Purpose or question | Suggestions used | How I checked the results |
|---|---|---|---|
| ChatGPT | Brainstorming ideas, correcting grammar, improving formatting, and giving suggestions | I used some of the suggested wording, grammar corrections, and formatting changes. | I reviewed all suggestions and kept only those I agreed with. The code, testing and results are my own work. |

### 7. Weekly Reflection and Next Step

The initialization issue showed me that valid command-line input
must also satisfy assumptions inside the simulation. Checking the
initial radius directly helped separate initialization behavior
from movement during later steps.

I also learned that images can look almost identical even when
the recorded collision counts differ. I used CSV results and file
hashes alongside visual inspection, rather than judging collision
behavior from appearance alone.

The CleanCheck profile provided evidence that the project could
build without relying on the existing Debug build cache.

Next, I will complete my final reflection, prepare the narrated
demonstration and check the contents and buildability of the
submission ZIP.

My declaration: I confirm that this entry accurately records my progress and assistance received.

Student initials / signature: Zhenyi Zhu
Date: 4 November 2026

## Final Project Reflection

Project option: Galaxy Simulation

### 1. Development Summary

I developed the Galaxy project from the supplied starter structure.
The completed work includes stronger configuration validation,
bounded collision sampling, mass-weighted merging, complete CSV
reporting and initial-to-final summary information.

I ran controlled experiments covering two body counts, three
collision thresholds and three sampling budgets. I also checked
seeded repetition and compared final-frame images. Later work
addressed small-system initialization, renderer safety, tree
ownership and coincident-body insertion.

### 2. Most Difficult Problem

The small-system initialization issue required me to connect
command-line validation with assumptions inside the simulation.
A system size of 0.05 AU passed validation, but the original fixed
inner radius of 0.1 AU exceeded the distribution's upper bound.

The revised lower radius, min(0.1, systemSizeAU * 0.5), gives a
valid range for the tested small systems while preserving the
original lower bound for larger systems. I checked initialization
directly and then ran a short simulation. This showed me why
valid input alone does not guarantee correct internal behavior.

### 3. Most Valuable Test

The coincident-body regression was particularly valuable because
the existing tests passed without covering this tree-insertion case.

The new test expects coincident positions to remain in one leaf,
checks their combined gravitational effect on another body, and
verifies that the original bodies are not retired. It also runs
two simulation steps with collisions disabled and expects two
active bodies, zero merges and preserved total mass.

The new case passed on Windows, alongside all 12 existing reporting
cases. This demonstrated the value of choosing tests from specific
failure conditions rather than relying only on a passing test count.

### 4. Code Understanding

The function I would use to explain the collision implementation is
checkAndMergeCollision. It rejects self-pairs and retired bodies,
handles the disabled threshold, validates active-body data and
checks the distance against a positive inclusive threshold.

For an eligible collision, it calculates the combined mass and
mass-weighted position, velocity and acceleration before changing
either body. It updates the survivor and clears the second body
without erasing a vector element.

The distinction between collision merging and tree aggregation is
also important: collision merging changes the active simulation
state, whereas coincident-position aggregation only changes the
tree's representation for force calculation.

### 5. Final AI-Use Summary

I used ChatGPT for Brainstorming ideas, correcting grammar, improving formatting, and giving suggestions. 
I used some of the suggested wording, grammar corrections, and formatting changes.
I reviewed all suggestions and kept only those I agreed with. The code, testing and results are my own work.

### 6. Future Improvement

I would next automate the experiment matrix using unique output
directories and filenames. This would reduce the risk of overwriting
CSV files, frames or videos and make repeated experiments easier.

I would also use repeated timing measurements before making
performance claims, and extend tree tests to very close but distinct
positions and extreme numeric ranges. The current tests do not
establish correctness or long-term stability for every configuration.

### Final Submission Check

- [✔] I have completed and dated my journal entries.
- [✔] I have included evidence relating to the work I describe.
- [✔] I have accurately recorded the AI and external help I used.
- [✔] I have verified that my project builds from a clean configuration.
- [✔] I can explain and modify the submitted code.
- [✔] I have excluded build folders, caches and unnecessary generated files from my project ZIP.

Student signature: Zhenyi Zhu    Date: 4 November 2026
