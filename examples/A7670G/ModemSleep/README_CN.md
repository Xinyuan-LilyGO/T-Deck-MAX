# ModemSleep

通过 `AT+CSCLK` 启用 A7670G 睡眠，拉高 DTR，让 ESP32-S3 深睡 30 秒，然后唤醒两者。

- 硬件：开发板、A7670G、电池或大电流电源；SIM 卡可选。
- 配置：需要时修改 `SLEEP_SECONDS`。
- 编译：设置 `src_dir = examples/A7670G/ModemSleep`。
- 预期：DTR 为高时模组停止响应，定时唤醒并拉低 DTR 后恢复响应。
- 测量：USB 供电和板上其他外设会显著影响电流；本例只演示时序，不承诺整板功耗数值。

改编自 `LilyGo-Modem-Series/examples/ModemSleep`（MIT）。
