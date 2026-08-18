# ReadBattery

Reads the voltage seen at the A7670G VBAT input with `AT+CBC` every five seconds.

- Hardware: board, A7670G and battery/high-current supply. A SIM and antenna are optional.
- Configuration: none.
- Build: set `src_dir = examples/A7670G/ReadBattery`.
- Expected result: modem supply voltage is printed in millivolts.
- Notes: this is modem VBAT, not fuel-gauge state of charge. Some firmware/hardware revisions do not expose a valid `AT+CBC` voltage; use the board fuel-gauge example for battery percentage.

Adapted from `LilyGo-Modem-Series/examples/ReadBattery` (MIT), using the target board's supported modem reading only.
