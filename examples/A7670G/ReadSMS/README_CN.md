# ReadSMS

列出 A7670G 中保存的短信，之后保持 AT 命令透传。

- 硬件：开发板、A7670G、LTE 天线、足够的电源和支持短信的 SIM 卡。
- 配置：SIM 有 PIN 锁时填写 `SIM_PIN`。
- 编译：设置 `src_dir = examples/A7670G/ReadSMS`。
- 预期：串口输出已存短信；可用 `AT+CMGR=<序号>` 查看单条短信。
- 隐私：串口内容包含发送号码与正文；本例不会删除短信。

改编自 `LilyGo-Modem-Series/examples/ReadSMS`（MIT）。
