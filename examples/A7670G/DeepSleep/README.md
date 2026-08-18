# DeepSleep

Powers the A7670G off, disables its power rail and puts the ESP32-S3 into timed deep sleep.

- Hardware: board, A7670G and battery/high-current supply. A SIM is optional.
- Configuration: change `SLEEP_SECONDS` if needed.
- Build: set `src_dir = examples/A7670G/DeepSleep`.
- Expected result: the board wakes every 30 seconds, restarts the modem, shuts it down and sleeps again.
- Measurement: the display, GNSS, LoRa and other rails are outside this example; whole-board current depends on their state.

Adapted from `LilyGo-Modem-Series/examples/DeepSleep` (MIT).
