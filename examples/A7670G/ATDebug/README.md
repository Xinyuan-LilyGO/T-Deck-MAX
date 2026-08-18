# ATDebug

Provides a bidirectional USB serial to A7670G AT-command bridge.

- Hardware: T-Deck Pro MAX, A7670G, battery/high-current supply and LTE antenna. A SIM is optional.
- Configuration: no sketch constants need editing. Use 115200 baud and `Both NL & CR` in the serial monitor.
- Build: set `src_dir = examples/A7670G/ATDebug` in `platformio.ini`.
- Expected result: the monitor reports that the modem is online; entering `AT` returns `OK`.
- Caution: unknown AT commands can change persistent modem settings. The helper skips the PWRKEY pulse when the modem already responds.

Adapted from `LilyGo-Modem-Series/examples/ATdebug` (MIT).
