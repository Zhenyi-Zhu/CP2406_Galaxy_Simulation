# CP2406 Weekly Project Development Journal

Working draft following Appendix B of the Galaxy task sheet. Entry 1 records the actual first development period; later entries will be completed at later checkpoints, not backdated as fictitious weeks. Transfer this content into the coordinator's required journal format/checkpoint submission if specified. This Markdown draft is not a claim that the coordinator has approved Markdown as the final journal format.

- Student name: Zhenyi Zhu
- Student number: [Student to add]
- Project option: Galaxy Simulation
- Teaching period: 2026 TR3

## Weekly Journal Entry 1

- Week / dates: 27–29 October 2026 (first development checkpoint)
- Project option: Galaxy Simulation
- Checkpoint / task focus: establish the baseline, validate configuration and begin/test the collision feature.

### 1. Goals for This Week

Build the supplied C++20 project in Windows/CLion, run a small baseline, locate outputs, preserve progress in GitHub, improve `parseArguments`/`SimulationConfig::validate`, and implement/test one-pair merging before broader experiments.

### 2. Development Work Completed

| File / component | Function or area changed | Work and attribution |
|---|---|---|
| `CMakeLists.txt` | Build targets | Windows-compatible Galaxy source list configured with AI guidance; student built/reran in CLion. `ConfigTests` added on 28 October and `CollisionTests` on 29 October. |
| `SimulationConfig.cpp/.h` | Parsing, validation, help | AI-assisted implementation rejects partial numeric inputs, negative counts, overflow, non-finite numbers and missing/blank values. Student ran the 142-check suite and selected CLI failures on Windows. |
| `main.cpp` | Exception handling | Dedicated help signal replaced the catch that previously treated all runtime errors as successful exits. Windows help output and exit 0 were verified. |
| `Collisions.cpp/.h` | Pair merge and bounded sampling | AI-assisted implementation calculates mass-weighted state, retires the second body, counts attempts/valid pairs/merges and uses a seeded engine. Assistant's Linux checks passed; student Windows verification is still pending. |
| README and test records | Setup, progress, evidence | Documented actual setup problems and results. README moved to repository root at the student's request. |

### 3. Evidence of Progress

Dated evidence filenames / links:

- [27 October: Windows CMake and baseline commit](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/0fbab45e6a3790e183c4a52ba547187f78e65fb4).
- [28 October: parsing implementation and tests](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/46f49cfa46961187962af4d790e746e28d7f9758).
- [28 October: Windows test evidence recorded](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/aa45f53e4838e7f4259fd5be0758934265d6007f).
- [TESTING.md](TESTING.md): student-supplied Windows console results, including `PASS: 142 configuration checks`, error messages and exit codes.
- [COLLISION_TESTING.md](COLLISION_TESTING.md): 29 October assistant-run Linux pair, sampling and integration checks. Student-run collision results will be added only after they are supplied.

The student did not save screenshots for the earlier configuration checks. Terminal output and saved versions are used instead; both are permitted evidence types in Appendix B. These records show progressive implementation and verification rather than merely a final screenshot.

### 4. Problem-Solving Record

On 27 October, the baseline generated 20 PPM frames but could not locate FFmpeg. The first WinGet attempt failed because of a source certificate error. Specifying the WinGet source reached the download/extraction stage, but extraction failed. With AI guidance, the student downloaded/extracted FFmpeg manually and placed `ffmpeg.exe` in the configured working directory. The next run reported `Created galaxy.mp4`, and the student confirmed playback.

This separated a video-conversion dependency failure from the already-working C++ simulation. The executable and generated files were excluded from commits.

A second issue was found during code inspection: conversion functions could accept a numeric prefix such as `200abc`, and unsigned conversion could mishandle negative input. The implementation now validates the whole input and checks the destination range. The Windows suite/manual checks supplied on 28 October verified the intended rejection paths.

### 5. Testing Performed

| Test / input | Expected result | Actual result | Pass / fail and action |
|---|---|---|---|
| Baseline: 200 bodies, 20 steps, seed 2406, samples 0 | Frames and video with working FFmpeg | 20 frames; MP4 generated; playback confirmed on 27 October | Pass (Windows); retain baseline for regression |
| `ConfigTests` | All focused checks pass | `PASS: 142 configuration checks`; exit 0 | Pass (Windows, 28 October) |
| `--bodies -1` | Reject negative count | Clear non-negative-integer message; exit 1 | Pass (Windows) |
| `--seed 4294967296` | Reject uint32 overflow | `value out of range for --seed`; exit 1 | Pass (Windows) |
| `--output --steps 2` | Detect missing output value | `missing value for --output`; exit 1 | Pass (Windows) |
| `--help` | Help, no simulation, exit 0 | Expanded help and exit 0 | Pass (Windows) |
| Hand-computed pair and collision edge/sampling tests | Conserved state, correct boundaries, bounded loop | 18 cases passed in assistant Linux environment | Pass (Linux, 29 October); student Windows check pending |

### 6. Generative AI and External Help Log

| Tool / person | Purpose or question | Suggestion used or rejected | How it was checked / changed |
|---|---|---|---|
| ChatGPT / Codex | Environment and FFmpeg troubleshooting | Corrected CMake and local FFmpeg executable approach used after WinGet failure | Student built and ran Galaxy; supplied console output; confirmed MP4 playback |
| ChatGPT / Codex | Implement safer CLI parsing and tests | AI-generated code and testing suggestions used | Assistant compiled/tested on Linux; student ran ConfigTests and manual CLI checks on Windows |
| ChatGPT / Codex | Implement collision merging/sampling | AI-generated implementation and focused tests used | Assistant checked hand-derived results, conservation, bounded attempts and seeded repetition; student collision verification pending |
| Lecturer task sheet, hint guide and installation guide | Clarify requirements, boundary choices and document structure | Use the supplied interfaces, Appendix A and Appendix B; choose/document zero threshold as disabled merging | Compared requirements with source and tests; full final requirements still need a completion audit |

### 7. Weekly Reflection and Next Step

Reflection to complete in the student's own words after review:

- [What did you learn from separating the simulation failure checks from FFmpeg conversion?]
- [Explain why exit code 1 is expected for `--bodies -1`, while the test executable exits 0.]
- [Identify one function you can now explain and one small change you can make yourself.]

Next steps: run `CollisionTests` in Windows, explain the two-body hand calculation, then implement full statistics/CSV reporting and the controlled experiments. Also address the starter's fixed inner sampling radius before using system sizes below 0.1 AU.

Student declaration: This entry accurately records my progress and assistance received. [Student to review before signing.]

Student initials / signature: ____________________    Date: ____________________

## Weekly Journal Entry 2

Week / dates: [Complete at the next actual checkpoint]

Project option: Galaxy Simulation

Checkpoint / task focus: [Complete when work is performed]

### 1. Goals for This Week

[Specific planned functions, features and tests.]

### 2. Development Work Completed

| File / component | Function or area changed | Summary of your own changes / assistance |
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

| Tool / person | Purpose or question | Suggestion used or rejected | How you checked / changed it |
|---|---|---|---|
| [Complete] | [Complete] | [Complete] | [Complete] |

### 7. Weekly Reflection and Next Step

[Personal learning, what you can explain/modify and the next specific task.]

Student declaration: This entry accurately records my progress and assistance received.

Student initials / signature: ____________________    Date: ____________________

## Weekly Journal Entry 3

Week / dates: [Complete at the next actual checkpoint]

Project option: Galaxy Simulation

Checkpoint / task focus: [Complete when work is performed]

### 1. Goals for This Week

[Specific planned functions, features and tests.]

### 2. Development Work Completed

| File / component | Function or area changed | Summary of your own changes / assistance |
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

| Tool / person | Purpose or question | Suggestion used or rejected | How you checked / changed it |
|---|---|---|---|
| [Complete] | [Complete] | [Complete] | [Complete] |

### 7. Weekly Reflection and Next Step

[Personal learning, what you can explain/modify and the next specific task.]

Student declaration: This entry accurately records my progress and assistance received.

Student initials / signature: ____________________    Date: ____________________

## Final Project Reflection

Project option: Galaxy Simulation

### 1. Development Summary

[Complete at the end; distinguish personally implemented work from AI assistance.]

### 2. Most Difficult Problem

[Describe the problem and resolution in your own words.]

### 3. Most Valuable Test

[Identify the test, expected result, actual result and why it was valuable.]

### 4. Code Understanding

[Name one function you can explain and modify, including its key decisions.]

### 5. Final AI-Use Summary

[Accurately summarise tools, affected work, verification and changes/rejections.]

### 6. Future Improvement

[Explain the next improvement and why it matters.]

### Final Submission Check

- [ ] Journal entries are complete and dated.
- [ ] Evidence is included and relates to the stated work.
- [ ] AI and external help are transparently recorded.
- [ ] Project builds from a clean configuration.
- [ ] I can explain and modify the submitted code.
- [ ] Project ZIP excludes build folders, caches and unnecessary generated files.

Student signature: ____________________    Date: ____________________
