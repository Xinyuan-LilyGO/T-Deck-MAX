# Network

输出模组信息、SIM CCID、注册状态、运营商、网络制式和信号质量。

- 硬件：开发板、A7670G、LTE 天线、足够的电源和已开通的 SIM 卡。
- 配置：仅在 SIM 启用 PIN 锁时填写 `SIM_PIN`。
- 编译：设置 `src_dir = examples/A7670G/Network`。
- 预期：完成网络注册，并每 5 秒输出一次 CSQ。
- 说明：CSQ 为 `99` 表示信号未知。本例不激活数据业务，因此不需要 APN。

改编自 `LilyGo-Modem-Series/examples/Network`（MIT）。
