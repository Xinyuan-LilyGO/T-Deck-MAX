# ModemPowerOff

Requests an orderly A7670G shutdown with `AT+CPOF`, then disables its XL9555-controlled power rail.

- Hardware: board, A7670G and battery/high-current supply. A SIM is optional.
- Configuration: none.
- Build: set `src_dir = examples/A7670G/ModemPowerOff`.
- Expected result: the modem shuts down and remains off until the board is reset.
- Caution: do not remove the rail before `AT+CPOF` completes; abrupt removal can lose modem state.

Adapted from `LilyGo-Modem-Series/examples/ModemPowerOff` (MIT).
