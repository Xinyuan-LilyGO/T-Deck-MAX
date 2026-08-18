# Network

Prints modem identity, SIM CCID, registration state, operator, radio mode and signal quality.

- Hardware: board, A7670G, LTE antenna, adequate power and an active SIM.
- Configuration: set `SIM_PIN` only when the SIM is PIN-locked.
- Build: set `src_dir = examples/A7670G/Network`.
- Expected result: registration completes and CSQ is printed every five seconds.
- Notes: `99` means signal quality is unknown. APN is not needed because this example does not activate packet data.

Adapted from `LilyGo-Modem-Series/examples/Network` (MIT).
