# Hardware reference

This mapping is taken from the firmware in this repository. It is the analog-QTR Nano build, not a generic five-digital-sensor/L298N pinout.

| Signal | Arduino Nano pin |
| --- | --- |
| Left motor forward input | D5 |
| Left motor reverse input | D6 |
| Right motor forward input | D9 |
| Right motor reverse input | D10 |
| Sensor 0 through sensor 4 | A1, A2, A3, A4, A5 |
| Calibration indicator | Built-in LED |

The code drives four H-bridge inputs with PWM. The exact motor-driver model is not identified in the source. Driver enable/standby wiring and battery/regulator details must be confirmed against the actual assembly before using this as a complete wiring guide. Motors require a motor driver and a suitable supply; they do not connect directly to GPIO pins.

Sensor order and motor polarity determine steering direction. If the mounted array runs in the opposite order, reverse the sensor-pin array or correct the physical wiring after a wheels-raised check. Do not change both at once.

## Hardware acceptance

1. Confirm the motor-driver model and its enable/standby connections.
2. With wheels raised, verify forward/reverse commands on both motors.
3. Sweep the array over black and white during calibration.
4. At low base speed, test a straight line and gentle curves.
5. Move the line to each side and check that the corrective turn heads toward it.
6. Remove the line: verify the recovery turn and stop after 750 ms.
7. Reintroduce the line: verify that tracking resumes.
8. Record settings, battery voltage, track surface, and a short demonstration video.

The supplied photographs and video document the earlier hardware run. They are not evidence that the maintained recovery revision has completed this acceptance procedure.
