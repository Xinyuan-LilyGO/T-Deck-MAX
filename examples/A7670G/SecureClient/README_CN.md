# SecureClient

演示 HTTPS 加密传输及 A7670G SSL 配置命令。

- 硬件：开发板、A7670G、LTE 天线、足够的电源和已开通流量的 SIM 卡。
- 配置：替换 `APN`，也可修改 `URL`。
- 编译：设置 `src_dir = examples/A7670G/SecureClient`。
- 预期：串口输出 HTTPS 状态与响应正文。
- 安全：`authmode=0` 只加密但不验证服务器。量产时必须写入正确 CA、绑定到 SSL 上下文 0，并设置 `authmode=1`；凭据、升级和控制流量不能关闭验证。

改编自 `LilyGo-Modem-Series/examples/SecureClient`（MIT）。目标工程 TinyGSM 的 A7670 驱动没有 `TinyGsmClientSecure` 类型，因此改用模组内置 HTTPS API。
