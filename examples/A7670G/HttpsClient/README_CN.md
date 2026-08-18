# HttpsClient

使用 A7670G 模组内置 HTTP/TLS 服务发起 HTTPS GET。

- 硬件：开发板、A7670G、LTE 天线、足够的电源和已开通流量的 SIM 卡。
- 配置：替换 `APN`，也可修改 `URL`。
- 编译：设置 `src_dir = examples/A7670G/HttpsClient`。
- 预期：串口输出 HTTP 状态码、响应头和 JSON 正文。
- 说明：TLS 能力受模组固件、时钟和服务器加密套件影响；本基础示例未写入私有 CA 包。

改编自 `LilyGo-Modem-Series/examples/HttpsClient`（MIT）。
