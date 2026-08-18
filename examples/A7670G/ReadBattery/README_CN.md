# ReadBattery

每 5 秒通过 `AT+CBC` 读取 A7670G VBAT 输入端看到的电压。

- 硬件：开发板、A7670G、电池或大电流电源；SIM 卡和天线可选。
- 配置：无需修改。
- 编译：设置 `src_dir = examples/A7670G/ReadBattery`。
- 预期：串口输出模组供电电压，单位为 mV。
- 说明：该值是模组 VBAT，不是电量计的荷电百分比；部分固件/硬件版本不返回有效 `AT+CBC` 电压，电池百分比应使用本项目电量计示例。

改编自 `LilyGo-Modem-Series/examples/ReadBattery`（MIT），仅保留目标板支持的模组电压读取。
