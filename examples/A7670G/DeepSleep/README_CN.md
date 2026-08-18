# DeepSleep

正常关闭 A7670G 和模组电源轨，然后让 ESP32-S3 定时深睡。

- 硬件：开发板、A7670G、电池或大电流电源；SIM 卡可选。
- 配置：需要时修改 `SLEEP_SECONDS`。
- 编译：设置 `src_dir = examples/A7670G/DeepSleep`。
- 预期：开发板每 30 秒唤醒，重新启动并关闭模组，之后再次进入深睡。
- 测量：本例不管理屏幕、GNSS、LoRa 等其他电源轨，整板电流取决于这些外设状态。

改编自 `LilyGo-Modem-Series/examples/DeepSleep`（MIT）。
