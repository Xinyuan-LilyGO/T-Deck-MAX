# A7670G examples for T-Deck Pro MAX

These examples are adapted from `LilyGo-Modem-Series` for the T-Deck Pro MAX hardware. They use the board's XL9555 expander for the modem power rail, PWRKEY and audio routing, and use GPIO 11/10 for the modem UART. Do not replace `A7670GBoard.h` with the source repository's multi-board `utilities.h`.

## Examples

| Example | Purpose |
| --- | --- |
| `ATDebug` | Interactive AT command bridge |
| `Network` | SIM, registration, operator and signal information |
| `SendSMS` / `ReadSMS` | Send and inspect SMS messages |
| `VoiceCalls` | Place a voice call through the modem audio path |
| `LBS` | Obtain a cell-based location |
| `HttpClient` / `HttpsClient` | HTTP and modem-native HTTPS requests |
| `MqttClient` | Modem-native MQTT publish and subscribe |
| `SecureClient` | TLS transport and certificate-validation guidance |
| `TCPClientMultiple` | Two simultaneous TCP sockets |
| `ModemSleep` / `ModemPowerOff` / `DeepSleep` | Modem and ESP32-S3 power management |
| `TextToSpeech` | A76XX text-to-speech through the modem audio path |
| `WakeupByRingOrSMS` | Wake the ESP32-S3 from the modem RI signal |
| `ReadBattery` | Read modem VBAT with `AT+CBC` |
| `ExternalGPS` | Parse the board's independent MIA-M10Q GNSS output |

## Build

Select exactly one sketch in `platformio.ini`, for example:

```ini
src_dir = examples/A7670G/Network
```

A battery or another supply capable of handling cellular current peaks is required. Install the LTE antenna before powering the modem. Examples that use network data require an active SIM and the correct APN.

## Compatibility notes

- A7670G does not provide the integrated GNSS available on some A7670 variants. `ExternalGPS` instead uses the T-Deck Pro MAX onboard MIA-M10Q.
- Voice, SMS, LBS, TTS, TLS and MQTT behavior depends on the exact A7670G suffix, modem firmware and mobile operator. Check the individual README before use.
- The `SecureClient` example encrypts transport but defaults to no CA validation. Production use must install the server CA and enable modem authentication mode 1.
- Camera, QWIIC, SimHat, SimShield, CAN, RS485, LoRa-shield and source-board power-monitor examples were not copied because their hardware is not present or uses different pins on T-Deck Pro MAX.

Source: `D:/dgx/code/4_github_lilygo/LilyGo-Modem-Series/examples`, adapted under the original MIT example license.
