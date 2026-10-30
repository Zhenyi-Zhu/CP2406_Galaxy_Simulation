# CP2406 Weekly Project Development Journal

- Student name: Zhenyi Zhu
- Student number: 14887350
- Project option: Galaxy Simulation
- Teaching period: 2026 TR3

## Weekly Journal Entry 1

- Week / dates:
- Project option: Galaxy Simulation
- Checkpoint / task focus:

### 1. Goals for This Week

I planned to build the supplied C++20 project in Windows/CLion, run a small baseline, locate outputs, preserve progress in GitHub, improve `parseArguments`/`SimulationConfig::validate`, and implement/test one-pair merging before broader experiments.

### 2. Development Work Completed

| File / component | Function or area changed | My work and assistance used |
|---|---|---|
| `CMakeLists.txt` |  |  |
| `SimulationConfig.cpp/.h` |  |  |
| `main.cpp` |  |  |
| `Collisions.cpp/.h` |  |  |
| README and test records |  |  |

### 3. Evidence of Progress

My dated evidence filenames / links:

- [27 October: Windows CMake and baseline commit](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/0fbab45e6a3790e183c4a52ba547187f78e65fb4).
- [28 October: parsing implementation and tests](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/46f49cfa46961187962af4d790e746e28d7f9758).
- [28 October: Windows test evidence recorded](https://github.com/Zhenyi-Zhu/CP2406_Galaxy/commit/aa45f53e4838e7f4259fd5be0758934265d6007f).
- [TESTING.md](TESTING.md): 
- [COLLISION_TESTING.md](COLLISION_TESTING.md):

### 4. Problem-Solving Record

On 27 October, I ran the baseline and generated 20 PPM frames, but the program could not locate FFmpeg. The first WinGet attempt failed because of a source certificate error. I specified the WinGet source and reached the download/extraction stage, but extraction failed. Following the troubleshooting suggestions, I downloaded and extracted FFmpeg manually and placed `ffmpeg.exe` in the configured working directory. My next run reported `Created galaxy.mp4`, and I confirmed that the video played successfully.

This helped me separate the video-conversion dependency failure from the already-working C++ simulation. I kept the executable and generated files out of my commits.

I addressed an input-validation issue: conversion functions could accept a numeric prefix such as `200abc`, while unsigned conversion could mishandle negative input. I incorporated the revised validation code, which checks the whole input and destination range. I ran the Windows test suite and manual checks on 28 October to verify the intended rejection paths.

### 5. Testing Performed

| Test / input | Expected result | Actual result | Pass / fail and action |
|---|---|---|---|
| Baseline: 200 bodies, 20 steps, seed 2406, samples 0 | Frames and video with working FFmpeg | 20 frames; MP4 generated; playback confirmed on 27 October | Pass (Windows); retain baseline for regression |
| `ConfigTests` | All focused checks pass | `PASS: 142 configuration checks`; exit 0 | Pass (Windows, 28 October) |
| `--bodies -1` | Reject negative count | Clear non-negative-integer message; exit 1 | Pass (Windows) |
| `--seed 4294967296` | Reject uint32 overflow | `value out of range for --seed`; exit 1 | Pass (Windows) |
| `--output --steps 2` | Detect missing output value | `missing value for --output`; exit 1 | Pass (Windows) |
| `--help` | Help, no simulation, exit 0 | Expanded help and exit 0 | Pass (Windows) |
| Hand-computed pair and collision edge/sampling tests | Conserved state, correct boundaries, bounded loop | 18 cases passed in Linux and in my Windows/CLion run; Windows exit 0 | Pass (Linux and Windows, 29 October); screenshot at 18:57:34 |

### 6. Generative AI and External Help Log

| Tool / person | My purpose or question | Suggestions I used or rejected | How I checked / changed them |
|---|---|---|---|
| ChatGPT/Codex | I sought brainstorming, technical advice, explanations, and grammar and formatting help. I also used generated/revised code and tests. | I incorporated help with setup, validation, collision implementation, testing and documentation. | I ran the configuration tests and selected CLI cases in Windows. I keep the tool-run Linux results separate, and I ran all 18 collision cases successfully in Windows on 29 October. |

### 7. Weekly Reflection and Next Step

My declaration: I confirm that this entry accurately records my progress and assistance received. 

Student initials / signature: ____________________    Date: ____________________

## Weekly Journal Entry 2

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

[I will summarise my development work and the assistance I used at the end of the project.]

### 2. Most Difficult Problem

[I will describe the problem and how I resolved it in my own words.]

### 3. Most Valuable Test

[Identify the test, expected result, actual result and why it was valuable.]

### 4. Code Understanding

[I will identify one function I can explain and modify, including its key decisions.]

### 5. Final AI-Use Summary

[I will summarise the tools I used, the work they supported, and how I verified, modified or rejected their output.]

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
