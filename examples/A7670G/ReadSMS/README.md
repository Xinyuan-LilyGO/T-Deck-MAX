# ReadSMS

Lists SMS records stored by the A7670G, then leaves an interactive AT bridge open.

- Hardware: board, A7670G, LTE antenna, adequate power and an SMS-capable SIM.
- Configuration: set `SIM_PIN` if required.
- Build: set `src_dir = examples/A7670G/ReadSMS`.
- Expected result: stored messages are printed. Use `AT+CMGR=<index>` to inspect one record.
- Privacy: serial output includes sender numbers and message contents. The example does not delete messages.

Adapted from `LilyGo-Modem-Series/examples/ReadSMS` (MIT).
