# Barycentre comparison — 1 November 2026

I compared two runs with 200 bodies, a system size of 50 AU,
20 steps, a timestep of 1024000 seconds, 2000 collision samples
per step, and seed 2406. Only the collision threshold changed.

Both runs started with the same barycentre:
(-1.9336542579693123, -0.0049923451558350485, 0) AU.

| Measurement | Collisions disabled (0 AU) | Collisions enabled (1 AU) |
|---|---:|---:|
| Attempted pairs | 40000 | 40000 |
| Valid pairs | 39808 | 36192 |
| Merged pairs | 0 | 13 |
| Final active bodies | 200 | 187 |
| Final barycentre x (AU) | -1.9849521004656021 | -1.984951896399926 |
| Final barycentre y (AU) | -0.052631727039141381 | -0.05263097265958732 |
| Barycentre displacement x (AU) | -0.051297842496289769 | -0.051297638430613635 |
| Barycentre displacement y (AU) | -0.047639381883306332 | -0.047638627503752272 |
| Elapsed time (ms) | 43.9416 | 40.8631 |

The initial and final total masses in both runs were reported as
4.0000000000000004e+29 kg. All initial and final barycentre z
coordinates were zero.

Both runs showed barycentre movement, including the run with
collisions disabled. Therefore, I cannot attribute the overall
movement to collisions alone. The displacement differences between
the two runs were approximately 2.04e-7 AU in x and 7.54e-7 AU in y.
This comparison alone does not establish momentum conservation
throughout the simulation.

I recorded the initial barycentres and displacements from the console
output. The CSV files currently contain only the final barycentres.

Sources:
- `n200_barycentre_disabled.csv`
- `n200_samples2000_rerun.csv`

The rerun file is an archived copy of the output originally written
to `n200_samples2000.csv`, so its experiment and output_file fields
retain their original names. The original sampling comparison uses
the earlier run with an elapsed time of 39.3373 ms.