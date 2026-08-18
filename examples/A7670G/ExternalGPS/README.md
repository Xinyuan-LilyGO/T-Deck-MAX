# ExternalGPS

Powers and parses the T-Deck Pro MAX independent onboard MIA-M10Q GNSS receiver. It does not use A7670G GNSS commands.

- Hardware: T-Deck Pro MAX and a correctly connected GNSS antenna; SIM and LTE antenna are not required.
- Configuration: default GNSS UART is 9600 baud on RX GPIO2/TX GPIO16.
- Build: set `src_dir = examples/A7670G/ExternalGPS`.
- Expected result: coordinates, satellite count and UTC are printed after a fix.
- Notes: first fix requires open sky and may take several minutes. An indoor or incorrectly connected antenna commonly produces NMEA data without a valid location.

Adapted from `LilyGo-Modem-Series/examples/ExternalGPS_A7670G_Only` (MIT); source L76K pin mapping was replaced by the board MIA-M10Q mapping.
