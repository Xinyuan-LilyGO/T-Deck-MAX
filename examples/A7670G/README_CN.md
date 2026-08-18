# T-Deck Pro MAX 的 A7670G 示例

本目录示例由 `LilyGo-Modem-Series` 适配而来。T-Deck Pro MAX 通过 XL9555 控制模组电源、PWRKEY 和音频通道，模组 UART 使用 GPIO 11/10，因此不能直接使用源仓库面向多款开发板的 `utilities.h`。

## 示例列表

| 示例 | 用途 |
| --- | --- |
| `ATDebug` | 交互式 AT 命令透传 |
| `Network` | 查看 SIM、注册状态、运营商与信号质量 |
| `SendSMS` / `ReadSMS` | 发送和读取短信 |
| `VoiceCalls` | 通过模组音频通道拨打电话 |
| `LBS` | 获取基站定位信息 |
| `HttpClient` / `HttpsClient` | HTTP 与模组内置 HTTPS 请求 |
| `MqttClient` | 使用模组内置 MQTT 收发消息 |
| `SecureClient` | TLS 传输及证书校验配置说明 |
| `TCPClientMultiple` | 同时使用两个 TCP Socket |
| `ModemSleep` / `ModemPowerOff` / `DeepSleep` | 模组与 ESP32-S3 低功耗控制 |
| `TextToSpeech` | 使用 A76XX TTS 和模组音频通道播报文本 |
| `WakeupByRingOrSMS` | 使用模组 RI 信号唤醒 ESP32-S3 |
| `ReadBattery` | 通过 `AT+CBC` 读取模组 VBAT |
| `ExternalGPS` | 解析板载独立 MIA-M10Q 的 GNSS 数据 |

## 编译

在 `platformio.ini` 中只启用一个示例，例如：

```ini
src_dir = examples/A7670G/Network
```

蜂窝模组工作时必须接入电池或可承受蜂窝网络峰值电流的电源，并在上电前接好 LTE 天线。联网示例还需要可用的 SIM 卡和正确 APN。

## 兼容性说明

- A7670G 不包含部分 A7670 型号具备的内置 GNSS；`ExternalGPS` 使用 T-Deck Pro MAX 板载 MIA-M10Q。
- 语音、短信、LBS、TTS、TLS、MQTT 是否可用，取决于 A7670G 完整后缀、模组固件和运营商网络，请先查看各示例 README。
- `SecureClient` 默认只加密传输，不校验 CA；量产时必须写入服务器 CA 并启用模组认证模式 1。
- 相机、QWIIC、SimHat、SimShield、CAN、RS485、LoRa 扩展板及源开发板专用电源监测示例因硬件或引脚不同未迁移。

来源：`D:/dgx/code/4_github_lilygo/LilyGo-Modem-Series/examples`，按原示例 MIT 许可证适配。
