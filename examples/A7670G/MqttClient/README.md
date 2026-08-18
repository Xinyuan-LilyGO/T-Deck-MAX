# MqttClient

Uses the A7670G native MQTT commands to connect, subscribe and publish without PubSubClient.

- Hardware: board, A7670G, LTE antenna, adequate power, active SIM and data plan.
- Configuration: replace `APN`, broker, client ID and topic names. Use a unique client ID.
- Build: set `src_dir = examples/A7670G/MqttClient`.
- Expected result: one connection message is published; incoming messages on `TOPIC_RX` are printed.
- Security: the default public broker and port 1883 are unencrypted and unsuitable for secrets or production control.

Adapted from `LilyGo-Modem-Series/examples/MqttClient` (MIT).
