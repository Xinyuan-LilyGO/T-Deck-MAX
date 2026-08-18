# ModemSleep

Enables A7670G sleep with `AT+CSCLK`, raises DTR, deep-sleeps the ESP32-S3 for 30 seconds, then wakes both.

- Hardware: board, A7670G and battery/high-current supply. A SIM is optional.
- Configuration: change `SLEEP_SECONDS` if needed.
- Build: set `src_dir = examples/A7670G/ModemSleep`.
- Expected result: the modem stops answering while DTR is high and answers again after timer wake.
- Measurement: USB power and other board peripherals materially affect current. This example demonstrates sequencing, not a guaranteed whole-board current figure.

Adapted from `LilyGo-Modem-Series/examples/ModemSleep` (MIT).
