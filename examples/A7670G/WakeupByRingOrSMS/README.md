# WakeupByRingOrSMS

Keeps the A7670G powered while the ESP32-S3 deep-sleeps, then wakes on the modem RI falling edge/level.

- Hardware: board, A7670G, LTE antenna, adequate battery power and a SIM supporting calls or SMS.
- Configuration: none; send a call or SMS to the installed SIM after the sleep message appears.
- Build: set `src_dir = examples/A7670G/WakeupByRingOrSMS`.
- Expected result: RI on GPIO7 wakes the ESP32-S3 and the modem URC is shown for five seconds.
- Notes: RI behavior, voice and SMS support depend on firmware/operator configuration. A noisy or permanently-low RI causes immediate wakeups.

Adapted from `LilyGo-Modem-Series/examples/WakeupByRingOrSMS` (MIT).
