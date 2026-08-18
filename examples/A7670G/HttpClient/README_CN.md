# HttpClient

激活蜂窝数据，通过 TCP Socket 发起普通 HTTP GET，不依赖额外 HTTP 库。

- 硬件：开发板、A7670G、LTE 天线、足够的电源和已开通流量的 SIM 卡。
- 配置：替换 `APN`；可修改 `SERVER`、`RESOURCE` 和 `PORT`。
- 编译：设置 `src_dir = examples/A7670G/HttpClient`。
- 预期：HTTP 响应头和正文输出到 115200 波特率串口。
- 安全：HTTP 不加密，请勿传输凭据或隐私数据；加密传输请用 `HttpsClient`。

改编自 `LilyGo-Modem-Series/examples/HttpClient`（MIT）。
