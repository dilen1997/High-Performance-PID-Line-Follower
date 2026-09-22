# Control design

The Pololu library calibrates reflectance readings to 0–1000 and reports the position of a black line from 0 to 4000 for five sensors. The center target is 2000.

```text
error = 2000 - position
correction = Kp * error + Ki * bounded_sum(error) + Kd * (error - previous_error)
left_pwm = clamp(base_pwm - correction)
right_pwm = clamp(base_pwm + correction)
```

`Ki` is zero in the supplied tuning. The stored integral remains zero when disabled and is bounded if enabled. The derivative uses successive samples without dividing by elapsed time; the gains therefore depend on the sampling loop.

## Tracking states

| Observation | Action |
| --- | --- |
| At least one sensor sees the line | Track using the position estimate |
| All sensors see black | Reuse the previous valid position |
| No sensor sees the line, prior side known | Turn slowly toward the last side |
| No line for 750 ms | Stop both motors |
| No prior side known | Stop rather than choose an arbitrary direction |
| Line returns | Resume with derivative history reset |

Recovery uses unsigned elapsed-time arithmetic so it also works across `millis()` rollover. The timeout does not restart until a line is observed again.

## Changes from the original

The original compared calibrated readings against thresholds derived from raw calibration values. It also calculated a recovery direction and then overrode both motors with straight-ahead cruising speed. The maintained version uses one calibrated threshold and an explicit recovery timeout. It avoids accumulating an unbounded 16-bit integral when the integral gain is zero.

The original is retained under `legacy/` for comparison. Because the recovery policy changes motion, validate on hardware before treating this revision as tuned.

## Verification and remaining work

Host tests exercise steering sign, output limits, recovery direction, search timeout, reacquisition, long-running updates, and timer rollover. They do not simulate reflectance, wheel slip, motor current, sensor noise, or mechanical dynamics.

No competition placement, speed benchmark, fixed loop rate, or full PID tuning is claimed. Useful next measurements are lap time, recovery success rate, and oscillation at a documented battery voltage and track geometry.

Reference: [Pololu QTRSensors usage notes](https://pololu.github.io/qtr-sensors-arduino/md_usage.html).

The local AVR build succeeded with the pinned dependencies: 6,406 bytes of flash and 77 bytes of static RAM reported by PlatformIO. Heap allocation by the sensor library and runtime stack are additional RAM usage. One signed/unsigned comparison warning comes from the upstream QTRSensors library.
