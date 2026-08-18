# SecureClient

Demonstrates encrypted HTTPS transport and the A7670G SSL configuration commands.

- Hardware: board, A7670G, LTE antenna, adequate power, active SIM and data plan.
- Configuration: replace `APN` and optionally `URL`.
- Build: set `src_dir = examples/A7670G/SecureClient`.
- Expected result: an HTTPS status and response body are printed.
- Security: `authmode=0` encrypts traffic but does not authenticate the server. For production, upload the correct CA certificate, bind it to SSL context 0 and set `authmode=1`. Never disable validation for credentials, updates or control traffic.

Adapted from `LilyGo-Modem-Series/examples/SecureClient` (MIT). The target TinyGSM fork has no `TinyGsmClientSecure` typedef for A7670, so this version uses the modem-native HTTPS API.
