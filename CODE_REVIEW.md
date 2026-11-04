## 1. Student and Project Details

- Student name: Zhenyi Zhu
- Student ID: 14887350
- Project option: Galaxy Simulation

## 2. Pointer Member Access

For a valid `body* a`, I use `a->position.x` to access the x component of the position in the object pointed to by `a`. It is equivalent to `(*a).position.x`; the parentheses make the dereference happen before member access. The pointer must refer to a live object before it is dereferenced.

Code evidence: `Bhtree.cpp`, `Bhtree::singleInteract`, `target->accel.x += factor * rM.x`. Here the pointer lets the function update the original target body's acceleration. `Bhtree::insert` instead copies `*insertBod` into the node's `myBod`; the tree nodes do not retain pointers to the input vector elements.

## 3. Octree Child Selection

I traced how `Bhtree::insertIntoChild` tests child regions in the order UNW, UNE, USW, USE, DNW, DNE and DSW, then falls back to DSE. `Octant::contains` uses inclusive comparisons on all three axes. A point on a shared boundary can therefore belong to several children, and the first matching branch wins. At the exact parent center, UNW is the first match. In the DSE fallback, `insert` still performs its own containment check.

Code evidence: `Bhtree.cpp`, `Bhtree::insertIntoChild`; `Octant.cpp`, `Octant::contains` and `makeChild`. The original tree and force calculations have not been redesigned.

## 4. Distinct Collision Indices

I require two distinct indices so that each collision pair represents two different physical bodies. If the same object is merged with itself, its mass could be counted twice and then the survivor could be cleared when the second reference is retired. The sampler rejects equal indices before accessing a pair. The merge function independently checks `&a == &b`, so direct calls are also protected.

Code evidence: `Collisions.cpp`, `sampleAndMergeCollisions` and `checkAndMergeCollision`; `tests/CollisionTests.cpp`, `selfPair`.

## 5. Bounded Sampling

I use `attempt < attempts` as the loop condition. Every iteration increments `attemptedPairs`, draws two indices and consumes its attempt even if both indices are equal or a body is retired. Only distinct active pairs increment `validPairs`, and only successful merges increment `mergedPairs`. The loop never waits for a required number of successful collisions, which might never occur.

With fewer than two vector slots, the function returns zero attempts safely. With at least two slots, it performs exactly the requested attempts. Zero attempts skips sampling. Zero threshold disables merging, but requested attempts and eligible-pair statistics are still counted.

Code evidence: `Collisions.cpp`, `sampleAndMergeCollisions`; `tests/CollisionTests.cpp`, `rareCollisions`, `disabledSamplingPass`, `retiredSelections` and `tinyCollections`.

## 6. Conservation During Merge

I combine the original masses as `totalMass`. The survivor receives position and velocity weighted by `a.mass / totalMass` and `b.mass / totalMass`. These implement the center-of-mass and momentum equations; multiplying the new velocity by the combined mass recovers the original total momentum, up to floating-point rounding. The second body is reset to `body{}`, so its transferred mass is not counted again. The vector is not resized or erased, keeping indices and references stable throughout the pass.

Acceleration is also mass-weighted because `Simulation.cpp` applies the already-calculated acceleration after the collision pass. Keeping only the first body's acceleration would discard the second body's contribution to the subsequent velocity update. This is a step-integration choice, not a claim of exact physical accuracy.

All candidate values are calculated and checked before either input changes. Invalid data or overflowing total mass causes an exception without a partial merge. Positive thresholds use distance <= threshold; zero explicitly disables merging.

Code evidence: `Collisions.cpp`, `checkAndMergeCollision`; `Simulation.cpp`, `simulateOneStep`; `tests/CollisionTests.cpp`, `handCalculatedMerge` and `conservationAndStableStorage`.

Hand check: masses 2 and 3; positions (0,0,0) and (2,4,6); velocities (1,2,3) and (-1,4,1). Expected survivor mass 5, position (1.2,2.4,3.6), velocity (-0.2,3.2,1.8); retired mass 0. Both the Linux test and my Windows/CLion run on 29 October produced these values. All 18 Windows collision cases passed with exit code 0.

## 7. Reproducible Randomness

I use `std::mt19937 generator(seed)` as a local random engine and `std::uniform_int_distribution<std::size_t>` to map its output to valid vector indices. Recreating the engine and copying the same starting bodies makes repeated passes comparable without depending on shared global random state. The same seed and inputs are expected to repeat on the same compiler/standard-library setup; the distribution mapping need not be identical across different implementations.

Evidence from repeated run: `seededRepetition` uses two fresh copies, threshold 0.5, 2000 attempts and seed 2406. Linux/GCC and my Windows run each produced 2000 attempts, 101 valid pairs, 3 merges and 2 active bodies in both local runs, with exactly equal body states. Elapsed wall-clock time is excluded from equality comparisons. I completed the three-threshold/two-body-count matrix and a repeated seeded configuration on 31 October. The results are recorded in experiment_matrix.csv and discussed in README.

## 8. Refactoring Summary

I retained the module separation already present in the starter
and extended the implementation within those responsibilities.

- `SimulationConfig.cpp` handles command-line parsing and validation.
  Integer and floating-point helpers check complete inputs, ranges
  and finite values. Help requests are handled separately from errors.

- `Collisions.cpp` handles pair merging and bounded sampling.
  It checks pair eligibility, calculates the merged state before
  mutation, retires the second body and returns collision statistics.

- `Simulation.cpp` initializes bodies, builds the tree, advances
  the simulation and calculates summary metrics. Initialization now
  adapts the inner radius for small systems. The existing
  `calculateSummary` function is reused before and after the loop
  to avoid duplicating mass and barycentre calculations.

- `ExperimentResult` in `Simulation.h` carries the collision counts,
  final summary, elapsed time, initial total mass and initial
  barycentre. The initial values are preserved separately from
  the final values.

- `Renderer.cpp` remains responsible for writing PPM snapshots.
  It does not parse command-line arguments or write CSV reports.

- `CsvReporter.cpp` owns the 19-column CSV schema, text escaping,
  numeric formatting and output-error handling. It receives the
  configuration and results without running the simulation.
  The CSV currently records the final summary; initial values
  and barycentre displacement are printed in the console.

- `main.cpp` coordinates configuration, frame cleanup, initialization,
  simulation, reporting and video conversion. It prints the returned
  metrics and calculates displacement as final minus initial
  barycentre. It does not implement the force or collision algorithms.

The separate test executables cover configuration, collisions,
reporting and rendering. Reporting tests also exercise simulation summaries and
initialization, using temporary output directories without FFmpeg.

These changes extend the supplied structure. I did not redesign the original Barnes–Hut force calculation.

On 3 November, I strengthened two existing components.

In Renderer.cpp, I added finite-value and positive-view checks,
clipped off-screen coordinates before converting them to integer
pixels, and checked file-opening and writing failures. Invalid
input is rejected before opening the output file, preserving any
existing image. Finite off-screen bodies are skipped because they
remain valid simulation data.

In Bhtree.h, I explicitly deleted copy and move operations.
Each node owns and deletes its child nodes, so a shallow copy
would create multiple owners of the same pointers and risk double
deletion. The tree copies input body values; it does not take
ownership of the caller's body pointer.

On 4 November, I incorporated a further tree-insertion correction.
Exactly coincident bodies cannot be separated by recursive spatial
subdivision. A leaf now retains their exact shared position and
combines their gravitational mass, rejecting an overflowing sum.

This is a tree representation decision rather than a collision merge:
the simulation bodies remain independent, and collision statistics
are unchanged. If a body at a different position is subsequently
inserted, the leaf can subdivide normally.

## 9. Testing Evidence

| Test | Purpose | Input/setup | Expected result | Actual result | Pass/fail and code location |
|---|---|---|---|---|---|
| Zero or negative parameter | Reject invalid CLI settings | `--bodies -1`; config cases include zero steps/size/dt | Clear error; no simulation | Windows error for bodies and exit 1; 142 config checks passed | Pass; `SimulationConfig.cpp`, `tests/ConfigTests.cpp`; see `TESTING.md` |
| Zero collision threshold | Check documented disabled mode | Two coincident bodies of masses 2 and 3; threshold 0 | No merge; masses unchanged | Linux and Windows tests matched | Pass (Linux and Windows); `zeroThreshold` |
| Exact-threshold distance | Check inclusive positive boundary | Positions (0,0,0), (3,4,0); threshold 5 | Merge | Linux and Windows tests merged | Pass (Linux and Windows); `exactThreshold` |
| Retired body | Prevent repeat use of transferred mass | Pair with mass 0 or -1, either argument order | False; no mutation | Linux and Windows tests matched | Pass (Linux and Windows); `retiredBodies` |
| Deterministic seeded run | Repeat outcomes on the same build | Fresh fixture copies, seed 2406, 2000 attempts | Equal statistics and states | Linux and Windows: 2000/101/3/2 in each pair of runs | Pass (Linux and Windows); `seededRepetition` |
| Hand-calculated merge | Verify conserved quantities | The two-body example in section 6 | Mass 5 and calculated position/velocity | Linux and Windows values matched | Pass (Linux and Windows); `handCalculatedMerge` |
| CSV reporting | Verify schema, precision, escaping and error handling | Known values, quoted text, repeated writes and invalid output destinations | Correct 19-column output and clear failures | Reporting checks passed in Windows | Pass; `tests/ReportingTests.cpp`; see `TESTING.md` |
| Initial summary preservation | Preserve initial values after simulation movement | Two bodies with masses 2 and 3 at (1,2,3) and (3,4,5) | Initial mass 5 and barycentre (2.2,3.2,4.2), retained after movement | Initial values matched; final summary independently checked | Pass (Windows, 1 November); `tests/ReportingTests.cpp` |
| Initialization radius bounds | Check small-system initialization | 200 bodies at system sizes 0.05, 0.1, 0.2 and 50 AU | Correct count and radial bounds; finite velocities and positive masses | All four configurations passed | Pass (Windows, 2 November); `tests/ReportingTests.cpp` |
| Coincident-body tree and integration | Prevent repeated subdivision and preserve simulation bodies | Two coincident bodies, a third external body, and a two-step run with collisions disabled | Correct gravitational contribution, unchanged body masses, two active bodies and zero merges | New case passed; all 13 reporting cases passed with exit code 0 | Pass (Windows, 4 November); `tests/ReportingTests.cpp` |

My Windows verification includes 142 configuration checks and
18 collision test cases. After adding initial-summary and radius-bound
checks, all 12 reporting test cases passed on 2 November.

I also completed the controlled experiment matrix, a repeated seeded
run, the sampling-budget comparison and the final-frame visual
comparison. These provide integration evidence alongside the
automated tests.

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

Supporting records:
- [Configuration and reporting tests](CP2406_A3_Galaxy_Student_Template/TESTING.md)
- [Collision tests](CP2406_A3_Galaxy_Student_Template/COLLISION_TESTING.md)
- [Experiment matrix](CP2406_A3_Galaxy_Student_Template/experiment_matrix.csv)
- [Sampling comparison](CP2406_A3_Galaxy_Student_Template/sampling_comparison.csv)
- [Barycentre comparison](CP2406_A3_Galaxy_Student_Template/BARYCENTRE_COMPARISON.md)
- [Visual comparison](CP2406_A3_Galaxy_Student_Template/VISUAL_COMPARISON.md)

## 10. Challenge and Resolution

### Challenge encountered

The configuration accepted any finite, positive system size, but
`initialiseBodies` used a fixed inner radius of 0.1 AU. For a system
size such as 0.05 AU, the random distribution's lower bound was
greater than its upper bound. Valid command-line input therefore
did not guarantee a valid initialization range.

### How I investigated it

I reviewed the radius distribution in `Simulation.cpp` and compared
its bounds with the values accepted by configuration validation.
I separated initialization checks from the simulation so that
later movement would not affect the radius measurements.

### Solution implemented

I changed the inner radius to
`std::min(0.1, config.systemSizeAU * 0.5)`.

For a 0.05 AU system, this gives a sampling range of 0.025–0.05 AU.
System sizes of at least 0.2 AU retain the original 0.1 AU inner
radius, including the 50 AU configuration used in my experiments.

### Evidence that the solution works

I added a test covering system sizes of 0.05, 0.1, 0.2 and 50 AU.
It checks the body count, initial radial bounds, xy-plane positions,
finite velocities and finite positive masses.

All 12 reporting test cases passed in Windows/CLion on 2 November
with exit code 0. A separate 40-body simulation at 0.05 AU completed
three steps with a one-second timestep. It retained all 40 active
bodies, reported equal initial and final total mass, and generated
frames, a CSV report and a video.

Evidence:
- [Test record](CP2406_A3_Galaxy_Student_Template/TESTING.md#small-system-initialization--2-november-2026)
- [Small-system results](CP2406_A3_Galaxy_Student_Template/small_system_check.csv)

These checks verify the tested ranges and a short integration run, rather than long-term numerical stability.

## 11. Generative AI Use Statement

Option A — Generative AI was used.

- Tools: I used ChatGPT.
- Purpose: I used ChatGPT for brainstorming ideas, correcting grammar, improving formatting, giving suggestions, and fixing some code that I could not work out myself.
- Parts affected: The wording, grammar and formatting of my written work, and a few parts of the code that were revised with AI help.
- Verification: I ran and tested the revised code and checked that the results were correct. I reviewed the other suggestions and kept only those I agreed with.
- Changes I made: I used some of the suggested wording, grammar and formatting changes, and used AI-revised code in places where I could not solve the problem myself.
- How I demonstrated understanding: I understand the overall work and the suggestions I accepted

## 12. Final Student Declaration

I will check each statement after completing my final review:

- [✔] The explanations are written in my own words.
- [✔] The code references relate to my submitted project.
- [✔] The reported tests were performed on my implementation.
- [✔] I can explain and modify the submitted code.
- [✔] Any generative AI use has been declared accurately.

Student name: Zhenyi Zhu    Date: 4 November 2026
