# TCPClientMultiple

Opens two independent A7670G TCP sockets and reads both HTTP responses.

- Hardware: board, A7670G, LTE antenna, adequate power, active SIM and data plan.
- Configuration: replace `APN`; edit the two hosts/resources in `setup()` when needed.
- Build: set `src_dir = examples/A7670G/TCPClientMultiple`.
- Expected result: responses from socket mux 0 and mux 1 are streamed to the monitor, then both close.
- Notes: output can be interleaved by design. Both test requests are unencrypted HTTP.

Adapted from `LilyGo-Modem-Series/examples/TCPClientMultiple` (MIT).
