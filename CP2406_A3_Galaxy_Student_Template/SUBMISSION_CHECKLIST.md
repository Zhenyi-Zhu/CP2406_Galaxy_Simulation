# Assessment Task 3 — submission format and remaining work

Checked against the supplied `CP2406_Assessment_Task_3_2026_Galaxy(1).docx` and hint guide on 29 October 2026. These are the task-sheet requirements; coordinator-specific checkpoints or final journal file format must follow any separate instructions.

## Clean project ZIP

Include source, headers, CMakeLists.txt, tests or clear test evidence, experiment CSV, `CODE_REVIEW.md` and the completed development journal. Include the root README when packaging; it now lives one folder above the CLion source folder. Keep `CODE_REVIEW.md` in the top-level CLion project folder beside CMakeLists.txt.

Remove `cmake-build-*`, other build directories, IDE caches, generated frame sequences, temporary files and unnecessary videos/executables. FFmpeg is a separately installed dependency, not source to bundle.

Recommended name: `CP2406-A3-Option3-Zhenyi-Zhu-Code.zip`.

## Code review

Use Appendix A's 12 sections, answer all eight technical questions with project-specific code evidence, include the AI-use statement and complete the student declaration only after review. `CODE_REVIEW.md` is currently an AI-assisted working draft, not a signed final response.

## Development journal

Appendix B requires three dated entries and a final reflection. Each entry contains goals, development work, dated evidence, problem solving, tests with expected/actual results, assistance used, reflection and next step. Terminal output, test results and saved versions are acceptable; screenshots are not the only option.

`PROJECT_JOURNAL.md` is the working source following this structure. Transfer to the supplied journal template/format if required by the coordinator. Actual development began on 27 October; do not invent earlier weeks. Entry 1 is drafted, later entries and personal reflections remain incomplete. Journal is worth 5 of the total 30 marks.

## Video

Length: 3–5 minutes. Demonstrate your own solution and explain it.

- [ ] Show `--help`.
- [ ] Show a valid run.
- [ ] Show an invalid configuration.
- [ ] Show collision statistics.
- [ ] Show the experiment CSV.
- [ ] Show one before/after refactoring example.
- [ ] Explain one collision edge case.
- [ ] Explain one seeded reproducibility test.

Recommended name: `CP2406-A3-Option3-Zhenyi-Zhu-Video.mp4`.

The generated `galaxy.mp4` animation is not the narrated demonstration video. The installation guides describe animation conversion as optional; it is useful visual evidence but not a replacement for the assessed demonstration.

## Technical completion tracker

- [x] Baseline builds and runs; configuration help and selected failures verified on Windows.
- [x] 142 configuration checks pass in the assistant environment and on the student's Windows setup.
- [x] Pair merging and bounded sampling implemented; 18 focused collision cases pass in the assistant environment.
- [ ] Verify the collision update in Windows/CLion.
- [ ] Finish Task 1 investigation: change at least three parameters with other settings fixed; include an invalid case.
- [ ] Finish CSV: configuration, seed, elapsed time, collision counts, active count, mass, barycentre and status, with deterministic row order.
- [ ] Finish Task 3 matrix: at least three collision thresholds and two body counts, including disabled/rare/higher collision cases and one seeded repeat.
- [ ] Analyse runtime, active count, conservation, centre-of-mass movement and visible output using actual results.
- [ ] Review very small system sizes, integration and other outstanding edge cases.
- [ ] Complete the code review, all journal entries, personal reflection and declarations.
- [ ] Record the demonstration and check a clean build from the final ZIP.
