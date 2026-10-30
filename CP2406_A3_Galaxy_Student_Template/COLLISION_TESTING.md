# Collision tests — 29 October 2026

## Rules and scope

- Positive threshold: merge when Euclidean distance is <= threshold (positions in AU).
- Zero threshold: disable merging, including coincident bodies.
- Active body: mass > 0. Retire the second body by clearing its state and setting mass to zero; never erase a vector element during a pass.
- Attempt: one pair of index draws, even if the indices are equal or a body is retired.
- Valid pair: distinct indices and two active bodies. A valid pair need not be close enough to merge.
- With fewer than two slots, return zero attempts. Otherwise run exactly the requested attempts. Zero attempts skips sampling.
- Fixed-seed comparisons apply within the same compiler/standard-library implementation. Do not require timings or cross-library distribution mappings to match.

## Automated pair/pass tests

Source: `tests/CollisionTests.cpp`. These checks ran in Linux/GCC 13.3 with C++20. I also ran all 18 cases successfully in Windows/CLion on 29 October, with exit code 0. Tests throw on failure and remain effective in Release builds.

| Case | Input / purpose | Expected | Actual |
|---|---|---|---|
| Hand calculation | Masses 2,3; positions (0,0,0),(2,4,6); velocities (1,2,3),(-1,4,1); threshold 8 | Mass 5; position (1.2,2.4,3.6); velocity (-0.2,3.2,1.8); retired mass 0 | Matched |
| Self-pair | Both references address the same body | False; unchanged | Passed |
| Retired bodies | Mass 0/-1 in either argument | False; unchanged | Passed |
| Outside threshold | Difference along z is 2; threshold 1 | False | Passed |
| Exact threshold | 3-4-5 displacement; threshold 5 | True | Passed |
| Just outside | Next representable x above 5; threshold 5 | False | Passed |
| Zero threshold | Coincident active bodies | No merge | Passed |
| Invalid threshold | Negative, infinity, NaN, including empty sampler | Exception; no mutation | Passed |
| Invalid body data | Infinite mass/position/velocity/acceleration | Exception; no mutation | Passed |
| Mass overflow | Two maximum-double masses | Exception; no mutation | Passed |
| Zero attempts | Five active bodies plus retired slot | Attempts/valid/merged = 0; active = 5 | Passed |
| Empty/single vector | 100 requested attempts | Zero actual attempts; correct active count | Passed |
| Rare collision | Three distant bodies; 5000 attempts | Terminate; no merges; self-pairs excluded | Passed |
| Disabled merging | Coincident bodies; threshold 0; 1000 attempts | 1000 attempts, no merges | Passed |
| Retired selections | One active plus two retired, then all retired | Valid pairs 0 | Passed |
| Seed repetition | Two fresh six-slot fixtures; seed 2406; threshold 0.5; 2000 attempts | Equal states/statistics | Both: attempts 2000, valid 101, merged 3, active 2 |
| Pass conservation/storage | Five active plus retired slot; threshold 100 | Mass/momentum/weighted position/force conserved; same vector storage | Passed; 4 merges, 1 active |
| Repeated retirement | Try to merge an already retired second body | No second merge or double counting | Passed |

CMake Release build completed and CTest passed both registered suites (2/2). The original 200-body, 20-step CLI baseline also completed, generating 20 frames, the placeholder CSV and an MP4.

Result: **18 collision test cases passed**. Existing **142 configuration checks passed**. Conservation uses relative/absolute tolerance of 1e-12; identical seeded runs compare every body field exactly within the same executable.

The acceleration check verifies that a merge retains the combined precomputed force contribution for the subsequent integration step. It does not claim globally exact conservation under the approximate gravity/integration model.

## Simulation integration checks

A temporary driver called the real `initialiseBodies` and `runExperiment` functions with 40 bodies, size 10 AU, 3 steps, dt 1000 seconds, 500 attempts per step and seed 2406. It used separate output directories and fresh body vectors.

- Threshold 100 AU: 1500 attempts, 39 valid pairs, 39 merges, 1 active body. Initial and final total mass were both approximately 4e29 kg (checked within relative tolerance 1e-12).
- Repeated threshold-100 run: exact match for collision counts, active count, total mass, barycentre and every body state; timings excluded.
- Threshold 0: 1500 attempts, no merges, 40 active bodies, total mass conserved.

These are integration checks, not the final controlled experiment matrix. Complete CSV fields were added on 30 October. The required three-threshold/two-body-count comparison matrix remains to be completed.

## Windows verification — 29 October 2026

I ran `CollisionTests.exe` in Windows/CLion. My console screenshot shows all 18 cases passing and exit code 0. The hand-calculated result was mass 5, position (1.2,2.4,3.6), velocity (-0.2,3.2,1.8), retired mass 0. The repeated seed-2406 test reported attempts 2000, valid 101, merged 3 and active 2; both runs were identical.

Evidence: my screenshot dated 29 October 2026, 18:57:34. This confirms the collision suite on Windows; it does not establish Windows verification of the reporting changes added the following day.

To repeat, reload CMake, select `CollisionTests` and run without arguments. The expected final line is `PASS: 18 collision test cases`. Valid-pair counts may differ across standard libraries; each local repeated run must match itself.
