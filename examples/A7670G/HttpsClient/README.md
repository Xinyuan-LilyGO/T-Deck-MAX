# HttpsClient

Performs an HTTPS GET with the A7670G modem-native HTTP/TLS service.

- Hardware: board, A7670G, LTE antenna, adequate power, active SIM and data plan.
- Configuration: replace `APN` and optionally `URL`.
- Build: set `src_dir = examples/A7670G/HttpsClient`.
- Expected result: HTTP status, response headers and JSON body are printed.
- Notes: TLS behavior depends on modem firmware, clock and server cipher support. This basic example does not install a private CA bundle.

Adapted from `LilyGo-Modem-Series/examples/HttpsClient` (MIT).
