# SendSMS

注册移动网络并发送一条文本短信。

- 硬件：开发板、A7670G、LTE 天线、足够的电源，以及支持短信业务的 SIM 卡/套餐。
- 配置：替换 `SMS_TARGET`，号码需包含国际区号；SIM 有 PIN 锁时填写 `SIM_PIN`。
- 编译：设置 `src_dir = examples/A7670G/SendSMS`。
- 预期：串口显示 `SMS sent.`，目标号码收到一条短信。
- 注意：每次烧录或复位都会再次发送并可能计费。短信能力取决于 A7670G 固件及运营商 IMS/短信网络。

改编自 `LilyGo-Modem-Series/examples/SendSMS`（MIT）。
