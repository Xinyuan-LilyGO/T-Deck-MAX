# HttpClient

Activates packet data, opens a TCP socket and performs a plain HTTP GET without extra HTTP libraries.

- Hardware: board, A7670G, LTE antenna, adequate power, active SIM and data plan.
- Configuration: replace `APN`; change `SERVER`, `RESOURCE` and `PORT` for your endpoint.
- Build: set `src_dir = examples/A7670G/HttpClient`.
- Expected result: response headers/body are streamed to the 115200-baud monitor.
- Security: HTTP is unencrypted. Do not send credentials or private data; use `HttpsClient` for encrypted transport.

Adapted from `LilyGo-Modem-Series/examples/HttpClient` (MIT).
