# TextToSpeech

Routes modem audio to the board amplifier and speaks one ASCII phrase with A76XX `AT+CTTS`.

- Hardware: board, TTS-capable A7670G firmware, speaker and adequate power. A SIM is optional.
- Configuration: replace `TEXT_TO_SPEAK`. The example uses mode 2 for ASCII text.
- Build: set `src_dir = examples/A7670G/TextToSpeech`.
- Expected result: the phrase plays through the modem audio path and `TTS complete.` is printed.
- Notes: TTS language packs and `AT+CTTS` support vary by modem suffix and firmware. Keep volume at a safe level.

Adapted from `LilyGo-Modem-Series/examples/TextToSpeech` (MIT).
