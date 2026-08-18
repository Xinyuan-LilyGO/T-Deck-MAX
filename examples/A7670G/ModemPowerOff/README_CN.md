# ModemPowerOff

先用 `AT+CPOF` 请求 A7670G 正常关机，再关闭 XL9555 控制的模组电源轨。

- 硬件：开发板、A7670G、电池或大电流电源；SIM 卡可选。
- 配置：无需修改。
- 编译：设置 `src_dir = examples/A7670G/ModemPowerOff`。
- 预期：模组关机并保持断电，复位开发板后才重新启动。
- 注意：必须等待 `AT+CPOF` 完成后再关闭电源轨，直接断电可能丢失模组状态。

改编自 `LilyGo-Modem-Series/examples/ModemPowerOff`（MIT）。
