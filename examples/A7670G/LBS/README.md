# LBS

Uses A7670G cell-based location service and prints coordinates, accuracy and UTC.

- Hardware: board, A7670G, LTE antenna, adequate power, active SIM and packet-data plan.
- Configuration: replace `APN`, and credentials when the operator requires them.
- Build: set `src_dir = examples/A7670G/LBS`.
- Expected result: an approximate location is printed every 30 seconds.
- Notes: LBS is network-derived, less accurate than GNSS, may expose location to the service, and is firmware/operator dependent.

Adapted from `LilyGo-Modem-Series/examples/LBSExample` (MIT).
