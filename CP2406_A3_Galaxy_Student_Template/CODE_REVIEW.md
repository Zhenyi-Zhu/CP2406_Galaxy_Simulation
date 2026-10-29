# Code Review — Galaxy Simulation

Working draft arranged in the 12 sections of Assessment Task 3 Appendix A. These AI-assisted technical notes must be reviewed, understood and rewritten where necessary by the student before submission. Personal understanding, reflections and declarations are not certified by this draft. Reporting and experiment work is still in progress.

## 1. Student and Project Details

- Student name: Zhenyi Zhu
- Student ID: [Student to add]
- Project option: Galaxy Simulation

## 2. Pointer Member Access

For a valid `body* a`, `a->position.x` accesses the x component of the position in the object pointed to by `a`. It is equivalent to `(*a).position.x`; the parentheses make the dereference happen before member access. The pointer must refer to a live object before it is dereferenced.

Code evidence: `Bhtree.cpp`, `Bhtree::singleInteract`, `target->accel.x += factor * rM.x`. Here the pointer lets the function update the original target body's acceleration. `Bhtree::insert` instead copies `*insertBod` into the node's `myBod`; the tree nodes do not retain pointers to the input vector elements.

## 3. Octree Child Selection

`Bhtree::insertIntoChild` tests child regions in the order UNW, UNE, USW, USE, DNW, DNE and DSW, then falls back to DSE. `Octant::contains` uses inclusive comparisons on all three axes. A point on a shared boundary can therefore belong to several children, and the first matching branch wins. At the exact parent centre, UNW is the first match. In the DSE fallback, `insert` still performs its own containment check.

Code evidence: `Bhtree.cpp`, `Bhtree::insertIntoChild`; `Octant.cpp`, `Octant::contains` and `makeChild`. The original tree and force calculations have not been redesigned.

## 4. Distinct Collision Indices

Two indices must refer to different vector elements to represent two physical bodies. If the same object is merged with itself, its mass could be counted twice and then the survivor could be cleared when the second reference is retired. The sampler rejects equal indices before accessing a pair. The merge function independently checks `&a == &b`, so direct calls are also protected.

Code evidence: `Collisions.cpp`, `sampleAndMergeCollisions` and `checkAndMergeCollision`; `tests/CollisionTests.cpp`, `selfPair`.

## 5. Bounded Sampling

The loop is controlled by `attempt < attempts`. Every iteration increments `attemptedPairs`, draws two indices and consumes its attempt even if both indices are equal or a body is retired. Only distinct active pairs increment `validPairs`, and only successful merges increment `mergedPairs`. The loop never waits for a required number of successful collisions, which might never occur.

With fewer than two vector slots, the function returns zero attempts safely. With at least two slots, it performs exactly the requested attempts. Zero attempts skips sampling. Zero threshold disables merging, but requested attempts and eligible-pair statistics are still counted.

Code evidence: `Collisions.cpp`, `sampleAndMergeCollisions`; `tests/CollisionTests.cpp`, `rareCollisions`, `disabledSamplingPass`, `retiredSelections` and `tinyCollections`.

## 6. Conservation During Merge

The original masses are combined as `totalMass`. The survivor receives position and velocity weighted by `a.mass / totalMass` and `b.mass / totalMass`. These implement the centre-of-mass and momentum equations; multiplying the new velocity by the combined mass recovers the original total momentum, up to floating-point rounding. The second body is reset to `body{}`, so its transferred mass is not counted again. The vector is not resized or erased, keeping indices and references stable throughout the pass.

Acceleration is also mass-weighted because `Simulation.cpp` applies the already-calculated acceleration after the collision pass. Keeping only the first body's acceleration would discard the second body's contribution to the subsequent velocity update. This is a step-integration choice, not a claim of exact physical accuracy.

All candidate values are calculated and checked before either input changes. Invalid data or overflowing total mass causes an exception without a partial merge. Positive thresholds use distance <= threshold; zero explicitly disables merging.

Code evidence: `Collisions.cpp`, `checkAndMergeCollision`; `Simulation.cpp`, `simulateOneStep`; `tests/CollisionTests.cpp`, `handCalculatedMerge` and `conservationAndStableStorage`.

Hand check: masses 2 and 3; positions (0,0,0) and (2,4,6); velocities (1,2,3) and (-1,4,1). Expected survivor mass 5, position (1.2,2.4,3.6), velocity (-0.2,3.2,1.8); retired mass 0. The Linux test produced these values. Windows collision-test verification is pending.

## 7. Reproducible Randomness

`std::mt19937 generator(seed)` provides a local random engine, and `std::uniform_int_distribution<std::size_t>` maps its output to valid vector indices. Recreating the engine and copying the same starting bodies makes repeated passes comparable without depending on shared global random state. The same seed and inputs are expected to repeat on the same compiler/standard-library setup; the distribution mapping need not be identical across different implementations.

Evidence from repeated run: `seededRepetition` uses two fresh copies, threshold 0.5, 2000 attempts and seed 2406. Linux/GCC produced 2000 attempts, 101 valid pairs, 3 merges and 2 active bodies in both runs, with exactly equal body states. Elapsed wall-clock time is excluded from equality comparisons. The full assessment experiment matrix remains to be completed.

## 8. Refactoring Summary

The supplied starter already separated these responsibilities; do not claim that all separation was newly implemented:

- Simulation responsibilities: `Simulation.cpp` initialises bodies, builds the tree, advances a step and calculates summary metrics.
- Renderer responsibilities: `Renderer.cpp` writes PPM snapshots.
- CsvReporter responsibilities: `CsvReporter.cpp` owns CSV output, but its placeholder row still needs replacement with the required experiment fields.
- Other changes completed: `SimulationConfig.cpp` now isolates integer/real conversion and validation helpers; `main.cpp` handles the dedicated `HelpRequested` signal separately from actual failures; `Collisions.cpp` implements merge and sampling behind the existing header interface. Separate test executables exercise configuration and collision behaviour without running the renderer or FFmpeg.

Before/after refactoring evidence for the final video and the completed reporting design: [Update after Task 3.]

## 9. Testing Evidence

| Test | Purpose | Input/setup | Expected result | Actual result | Pass/fail and code location |
|---|---|---|---|---|---|
| Zero or negative parameter | Reject invalid CLI settings | `--bodies -1`; config cases include zero steps/size/dt | Clear error; no simulation | Windows error for bodies and exit 1; 142 config checks passed | Pass; `SimulationConfig.cpp`, `tests/ConfigTests.cpp`; see `TESTING.md` |
| Zero collision threshold | Check documented disabled mode | Two coincident bodies of masses 2 and 3; threshold 0 | No merge; masses unchanged | Linux test matched | Pass (Linux); `zeroThreshold` |
| Exact-threshold distance | Check inclusive positive boundary | Positions (0,0,0), (3,4,0); threshold 5 | Merge | Linux test merged | Pass (Linux); `exactThreshold` |
| Retired body | Prevent repeat use of transferred mass | Pair with mass 0 or -1, either argument order | False; no mutation | Linux test matched | Pass (Linux); `retiredBodies` |
| Deterministic seeded run | Repeat outcomes on the same build | Fresh fixture copies, seed 2406, 2000 attempts | Equal statistics and states | Linux: 2000/101/3/2 in both runs | Pass (Linux); `seededRepetition` |
| Hand-calculated merge | Verify conserved quantities | The two-body example in section 6 | Mass 5 and calculated position/velocity | Linux values matched | Pass (Linux); `handCalculatedMerge` |

Full collision evidence and remaining validation: [COLLISION_TESTING.md](COLLISION_TESTING.md). Do not mark the new Windows collision tests as passed until run locally.

## 10. Challenge and Resolution

Documented candidate: FFmpeg installation failed during archive extraction on 27 October. A manual download and local executable resolved video creation; the student supplied successful console output and confirmed playback. Another candidate is rejecting partly numeric CLI inputs with full-token checks.

- Challenge encountered: [Student to select and describe in their own words.]
- How I investigated it: [Student to describe their actions, separating AI suggestions.]
- Solution implemented: [Student response.]
- Evidence that the solution works: [Choose the relevant console output/test/commit.]

## 11. Generative AI Use Statement

Option A — Generative AI was used. Draft factual record for student review:

- Tools: ChatGPT / Codex.
- Purpose: explain the template and setup, suggest and implement configuration validation, collision code, automated tests and documentation structure.
- Parts affected: CMake setup, `SimulationConfig.cpp/.h`, the help exception in `main.cpp`, `Collisions.cpp/.h`, test sources, README and draft test/review/journal documents.
- Verification: assistant-run Linux builds, focused tests and simulation checks; student-run Windows configuration suite and manual CLI tests documented in `TESTING.md`. Windows collision verification remains pending.
- Changes I made: [Student to state their actual edits/configuration actions. AI-authored implementation must not be described as independently written.]
- How I demonstrated understanding: [Student to add an explanation they can give and a modification they can make.]

## 12. Final Student Declaration

Leave unchecked until personally verified at completion:

- [ ] The explanations are written in my own words.
- [ ] The code references relate to my submitted project.
- [ ] The reported tests were performed on my implementation.
- [ ] I can explain and modify the submitted code.
- [ ] Any generative AI use has been declared accurately.

Student name: Zhenyi Zhu    Date: [Complete at final review]
