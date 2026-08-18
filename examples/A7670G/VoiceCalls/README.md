# VoiceCalls

Routes modem audio to the board amplifier, places one call and hangs up after 20 seconds.

- Hardware: board, A7670G voice-capable variant/firmware, speaker, LTE antenna, adequate power and voice SIM.
- Configuration: replace `PHONE_NUMBER`; adjust `CALL_DURATION_MS` if needed.
- Build: set `src_dir = examples/A7670G/VoiceCalls`.
- Expected result: the destination rings, audio uses the modem path, then the call ends.
- Caution: calls may be billed. Some A7670G suffixes or operator VoLTE profiles do not support voice.

Adapted from `LilyGo-Modem-Series/examples/VoiceCalls` (MIT).
