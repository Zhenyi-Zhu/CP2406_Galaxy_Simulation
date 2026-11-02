# Visual comparison — 2 November 2026

I compared the final rendered frames for collision thresholds of
0, 0.01 and 1 AU. Each run used 200 bodies, a system size of 50 AU,
20 steps, a timestep of 1024000 seconds, 1000 collision samples
per step and seed 2406.

The images below are frame_0019.ppm converted to PNG without resizing.
They show the state after 20 simulation steps, using the same
800 × 800 resolution and rendering scale.

| Threshold (AU) | Merges | Active bodies | Elapsed time (ms) |
|---|---:|---:|---:|
| 0 | 0 | 200 | 42.6983 |
| 0.01 | 0 | 200 | 44.6260 |
| 1 | 10 | 190 | 36.8967 |

## Collisions disabled — 0 AU

![Collisions disabled](visual_comparison/disabled_final.png)

## Low threshold — 0.01 AU

![Low collision threshold](visual_comparison/rare_final.png)

## Higher threshold — 1 AU

![Higher collision threshold](visual_comparison/high_final.png)

## Observations

The disabled and low-threshold final images are pixel-identical.
Their CSV reports also show zero merges and identical final mass
and barycentre values. The positive threshold allowed merging,
but no sampled pair merged in this run.

The higher-threshold image retains a similar overall distribution,
with small local differences in the plotted points. Its CSV records
10 merges and 190 remaining active bodies. The visual differences
are subtle at the original display size.

The PNG comparison found 23 changed pixel locations between the
disabled and higher-threshold final frames. The disabled and
low-threshold images each contain 199 non-black pixels; the
higher-threshold image contains 190.

Rendered pixel counts are not a reliable substitute for active-body
counts because projected points can overlap. I use the CSV statistics
to determine the number of active bodies and merges.

All three runs reported the same total mass of approximately
4 × 10^29 kg. These single-run timings include simulation and frame
output, so they do not establish that a higher threshold is faster.

## Evidence

- `visual_disabled.csv`
- `visual_rare.csv`
- `visual_high.csv`
- The three final-frame PNG files linked above.

This comparison concerns the final frames. It does not establish
long-term stability or describe every intermediate frame.
